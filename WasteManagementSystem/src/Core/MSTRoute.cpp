// MSTRoute.cpp
// Implementation of the MST route algorithm using Prim's algorithm
#include "pch.h"
#include "MSTRoute.h"

MSTRoute::MSTRoute()
    : Route("MST Route", 40.0f) // 40% waste threshold for MST route
{
}

MSTRoute::~MSTRoute()
{
}

std::vector<int> MSTRoute::FilterDestinations(const std::vector<WasteLocation>& locations)
{
    std::vector<int> filteredDestinations;

    // Add station as the first destination
    filteredDestinations.push_back(0);

    // Add locations that meet the waste threshold and are within distance limit
    for (const auto& location : locations) {
        if (location.GetWasteLevel() >= m_wasteThreshold) {
            int id = WasteLocation::dict_Name_toId[location.GetLocationName()];

            // Check if within maximum distance from station
            if (id > 0 && WasteLocation::map_distance_matrix[0][id] <= m_maxDistanceFromStation) {
                filteredDestinations.push_back(id);
            }
        }
    }

    return filteredDestinations;
}

std::vector<std::pair<int, int>> MSTRoute::BuildMST(const std::vector<int>& destinations)
{
    int n = destinations.size();

    // No MST possible if less than 2 nodes
    if (n < 2) {
        return {};
    }

    // Create a mapping from destination ID to index in the destinations vector
    std::unordered_map<int, int> idToIndex;
    for (int i = 0; i < n; i++) {
        idToIndex[destinations[i]] = i;
    }

    // Initialize MST data structures
    std::vector<bool> visited(n, false);
    std::vector<float> key(n, INF);
    std::vector<int> parent(n, -1);

    // Start with the first node (station)
    key[0] = 0;

    // Priority queue to get node with minimum key value
    std::priority_queue<std::pair<float, int>, std::vector<std::pair<float, int>>,
        std::greater<std::pair<float, int>>> pq;

    pq.push({ 0, 0 });

    // Run Prim's algorithm
    while (!pq.empty()) {
        int u = pq.top().second;
        pq.pop();

        if (visited[u]) {
            continue;
        }

        visited[u] = true;

        // Consider all adjacent vertices of u
        for (int v = 0; v < n; v++) {
            if (u == v) {
                continue;
            }

            // Get actual node IDs
            int uId = destinations[u];
            int vId = destinations[v];

            // Get distance between nodes
            float weight = WasteLocation::map_distance_matrix[uId][vId];

            // If no direct path, use a large value
            if (weight >= INF) {
                weight = 1000.0f; // Large but finite value
            }

            // Update key and parent if a shorter edge is found
            if (!visited[v] && weight < key[v]) {
                key[v] = weight;
                parent[v] = u;
                pq.push({ key[v], v });
            }
        }
    }

    // Construct MST from parent array
    std::vector<std::pair<int, int>> mst;
    for (int i = 1; i < n; i++) {
        // Add edge between parent[i] and i
        // Using actual node IDs
        mst.push_back({ destinations[parent[i]], destinations[i] });
    }

    return mst;
}

std::vector<std::vector<int>> MSTRoute::CreateAdjacencyList(
    const std::vector<std::pair<int, int>>& mst, int numNodes)
{
    // Initialize adjacency list
    std::vector<std::vector<int>> adjList(numNodes);

    // Add edges from MST
    for (const auto& edge : mst) {
        int u = edge.first;
        int v = edge.second;

        adjList[u].push_back(v);
        adjList[v].push_back(u); // Undirected graph
    }

    return adjList;
}

void MSTRoute::DFSTraversal(int node, std::vector<std::vector<int>>& adjList,
    std::vector<int>& tour)
{
    // Visit node
    tour.push_back(node);

    // Process all neighbors
    while (!adjList[node].empty()) {
        int next = adjList[node].back();
        adjList[node].pop_back();

        // Remove the reverse edge
        auto it = std::find(adjList[next].begin(), adjList[next].end(), node);
        if (it != adjList[next].end()) {
            adjList[next].erase(it);
        }

        // Recursively traverse the next node
        DFSTraversal(next, adjList, tour);
    }
}

std::vector<int> MSTRoute::GenerateEulerTour(const std::vector<std::pair<int, int>>& mst, int startNode)
{
    // If MST is empty, return only the start node
    if (mst.empty()) {
        return { startNode };
    }

    // Find the maximum node ID to determine size of adjacency list
    int maxNode = startNode;
    for (const auto& edge : mst) {
        maxNode = std::max(maxNode, std::max(edge.first, edge.second));
    }

    // Create adjacency list
    std::vector<std::vector<int>> adjList = CreateAdjacencyList(mst, maxNode + 1);

    // Generate Euler tour
    std::vector<int> tour;
    DFSTraversal(startNode, adjList, tour);

    return tour;
}

std::vector<int> MSTRoute::ShortcutEulerTour(const std::vector<int>& eulerTour)
{
    std::vector<int> shortcutTour;
    std::unordered_set<int> visited;

    // Add start node
    shortcutTour.push_back(eulerTour[0]);
    visited.insert(eulerTour[0]);

    // Process all nodes in Euler tour
    for (size_t i = 1; i < eulerTour.size(); i++) {
        int node = eulerTour[i];

        // Add node if not visited before
        if (visited.find(node) == visited.end()) {
            shortcutTour.push_back(node);
            visited.insert(node);
        }
    }

    // If tour doesn't end at station, add it
    if (shortcutTour.back() != 0) {
        shortcutTour.push_back(0);
    }

    return shortcutTour;
}

std::vector<float> MSTRoute::CalculateSegmentDistances(const std::vector<int>& route)
{
    std::vector<float> distances;

    // Calculate distances between consecutive locations
    for (size_t i = 0; i < route.size() - 1; i++) {
        float distance = WasteLocation::map_distance_matrix[route[i]][route[i + 1]];

        // If no direct path, use a large value
        if (distance >= INF) {
            distance = 1000.0f; // Large but finite value
        }

        distances.push_back(distance);
    }

    return distances;
}

bool MSTRoute::CalculateRoute(const std::vector<WasteLocation>& locations)
{
    // Clear previous route data
    m_finalRoute.clear();
    m_individualDistances.clear();
    m_totalDistance = 0.0f;
    m_timeTaken = 0.0f;
    m_fuelConsumption = 0.0f;
    m_wage = 0.0f;
    m_totalCost = 0.0f;

    // Filter destinations based on waste level and distance
    m_filteredDestinations = FilterDestinations(locations);

    // Check if any locations need pickup (besides the station)
    if (m_filteredDestinations.size() <= 1) {
        m_pickupRequired = false;
        return false;
    }

    m_pickupRequired = true;

    // Build MST
    std::vector<std::pair<int, int>> mst = BuildMST(m_filteredDestinations);

    // Generate Euler tour
    std::vector<int> eulerTour = GenerateEulerTour(mst, 0);

    // Shortcut Euler tour to get Hamiltonian path
    m_finalRoute = ShortcutEulerTour(eulerTour);

    // Calculate individual segment distances
    m_individualDistances = CalculateSegmentDistances(m_finalRoute);

    // Calculate costs
    CalculateCosts();

    return true;
}

const std::vector<int>& MSTRoute::GetFilteredDestinations() const
{
    return m_filteredDestinations;
}

bool MSTRoute::IsPickupRequired() const
{
    return m_pickupRequired;
}