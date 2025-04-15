/**
 * MSTRoute.cpp
 * Implementation of the Minimum Spanning Tree route algorithm using Prim's algorithm
 * for waste collection path optimization.
 */
#include "pch.h"
#include "MSTRoute.h"

MSTRoute::MSTRoute()
    : Route("MST Route", 40.0f), /* 40% waste threshold for MST route selection */
    m_pickupRequired(false) /* Initialize m_pickupRequired to false */
{
}

MSTRoute::~MSTRoute()
{
}

std::vector<int> MSTRoute::FilterDestinations(const std::vector<WasteLocation>& locations)
{
    std::vector<int> filteredDestinations;

    /* Start with the central station as the first destination */
    filteredDestinations.push_back(0);

    /* Filter locations based on waste threshold and maximum distance constraints */
    for (const auto& location : locations) {
        if (location.GetWasteLevel() >= m_wasteThreshold) {
            int id = WasteLocation::dict_Name_toId[location.GetLocationName()];

            /* Check if location is within the maximum distance range using direct distance */
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

    /* Early return if MST construction is not possible */
    if (n < 2) {
        return {};
    }

    /* Initialize data structures for Prim's algorithm */
    std::vector<bool> visited(n, false);
    std::vector<float> key(n, INF);
    std::vector<int> parent(n, -1);

    /* Begin with the station node */
    key[0] = 0;

    /* Min-priority queue for efficient vertex selection */
    std::priority_queue<std::pair<float, int>, std::vector<std::pair<float, int>>,
        std::greater<std::pair<float, int>>> pq;

    pq.push({ 0, 0 });

    /* Execute Prim's algorithm to build the MST */
    while (!pq.empty()) {
        int u = pq.top().second;
        pq.pop();

        if (visited[u]) {
            continue;
        }

        visited[u] = true;

        /* Examine all potential connections to unvisited vertices */
        for (int v = 0; v < n; v++) {
            if (u == v) {
                continue;
            }

            /* Map to actual location IDs */
            int uId = destinations[u];
            int vId = destinations[v];

            /* Use direct distance as edge weight */
            float weight = WasteLocation::map_distance_matrix[uId][vId];

            /* Update if a shorter connection is found */
            if (!visited[v] && weight < key[v]) {
                key[v] = weight;
                parent[v] = u;
                pq.push({ key[v], v });
            }
        }
    }

    /* Construct the final MST from parent references */
    std::vector<std::pair<int, int>> mst;
    for (int i = 1; i < n; i++) {
        /* Validate parent node before adding edge */
        if (parent[i] != -1) {
            /* Store edges using actual location IDs */
            mst.push_back({ destinations[parent[i]], destinations[i] });
        }
    }

    return mst;
}

std::vector<std::vector<int>> MSTRoute::CreateAdjacencyList(
    const std::vector<std::pair<int, int>>& mst, int numNodes)
{
    /* Find maximum vertex ID to properly size the adjacency list */
    int maxVertex = -1;
    for (const auto& edge : mst) {
        maxVertex = std::max(maxVertex, edge.first);
        maxVertex = std::max(maxVertex, edge.second);
    }

    /* Ensure numNodes is sufficient to include all vertices */
    numNodes = std::max(numNodes, maxVertex + 1);

    /* Create the adjacency list structure */
    std::vector<std::vector<int>> adjList(numNodes);

    /* Populate adjacency list with edges from MST */
    for (const auto& edge : mst) {
        int u = edge.first;
        int v = edge.second;

        /* Safety check to prevent out-of-bounds access */
        if (u >= 0 && u < numNodes && v >= 0 && v < numNodes) {
            adjList[u].push_back(v);
            adjList[v].push_back(u); /* Undirected graph representation */
        }
        else {
            /* Log error for invalid vertex indices */
            std::cerr << "Invalid vertex in MST edge: (" << u << ", " << v << ")" << std::endl;
        }
    }

    return adjList;
}

void MSTRoute::DFSTraversal(int node, std::vector<std::vector<int>>& adjList,
    std::vector<int>& tour, std::vector<bool>& visited) {
    /* Safety bounds check for node index */
    if (node < 0 || node >= static_cast<int>(visited.size())) {
        std::cerr << "Error: Node index " << node << " out of range in DFSTraversal" << std::endl;
        return;
    }

    /* Skip already visited nodes */
    if (visited[node]) {
        return;
    }

    /* Mark current node as visited and add to tour path */
    visited[node] = true;
    tour.push_back(node);

    /* Recursively visit all unvisited neighbors */
    if (node < static_cast<int>(adjList.size())) {
        for (int neighbor : adjList[node]) {
            if (neighbor >= 0 && neighbor < static_cast<int>(visited.size()) && !visited[neighbor]) {
                DFSTraversal(neighbor, adjList, tour, visited);
            }
        }
    }
}


std::vector<int> MSTRoute::GenerateEulerTour(const std::vector<std::pair<int, int>>& mst, int startNode) {
    /* Determine maximum vertex ID to properly size data structures */
    int maxVertex = -1;
    for (const auto& edge : mst) {
        maxVertex = std::max(maxVertex, edge.first);
        maxVertex = std::max(maxVertex, edge.second);
    }

    /* Allocate sufficient space for all vertices */
    int numNodes = maxVertex + 1;

    /* Build graph representation from MST */
    std::vector<std::vector<int>> adjList = CreateAdjacencyList(mst, numNodes);

    /* Validate start node and select alternative if invalid */
    if (startNode < 0 || startNode >= numNodes) {
        /* Fall back to first edge's source or 0 if MST is empty */
        startNode = mst.empty() ? 0 : mst[0].first;
        if (startNode < 0 || startNode >= numNodes) {
            /* Return empty tour if no valid start node exists */
            std::cerr << "Error: No valid start node found for Euler Tour" << std::endl;
            return std::vector<int>();
        }
    }

    /* Track visited nodes during traversal */
    std::vector<bool> visited(numNodes, false);

    /* Store the generated tour */
    std::vector<int> tour;

    /* Execute depth-first traversal to create Euler tour */
    DFSTraversal(startNode, adjList, tour, visited);

    return tour;
}

std::vector<int> MSTRoute::ShortcutEulerTour(const std::vector<int>& eulerTour)
{
    std::vector<int> hamiltonianPath;
    std::unordered_set<int> visitedNodes;

    for (int node : eulerTour) {
        /* Add only nodes that haven't been visited before */
        if (visitedNodes.find(node) == visitedNodes.end()) {
            hamiltonianPath.push_back(node);
            visitedNodes.insert(node);
        }
    }

    return hamiltonianPath;
}

std::vector<float> MSTRoute::CalculateSegmentDistances(const std::vector<int>& route)
{
    std::vector<float> distances;

    /* Calculate distances between each adjacent pair of nodes in the route */
    for (size_t i = 0; i < route.size() - 1; i++) {
        int from = route[i];
        int to = route[i + 1];
        float distance = WasteLocation::map_distance_matrix[from][to];
        distances.push_back(distance);
    }

    return distances;
}

std::vector<int> MSTRoute::PathReconstruction(int start, int end, const int matrix[9][9]) {
    std::vector<int> path;
    /* Reconstruct the shortest path from start to end using intermediate vertices */
    while (start != end) {
        path.push_back(start);
        start = matrix[start][end];
    }
    path.push_back(end);
    return path;
}

std::vector<int> MSTRoute::ExpandRouteWithIntermediateNodes(const std::vector<int>& basicRoute) {
    std::vector<int> expandedRoute;

    /* Add the starting node if the route is not empty */
    if (!basicRoute.empty()) {
        expandedRoute.push_back(basicRoute[0]);
    }

    /* Expand the route by adding intermediate nodes while avoiding duplicates */
    for (size_t i = 0; i < basicRoute.size() - 1; i++) {
        int start = basicRoute[i];
        int end = basicRoute[i + 1];

        /* Find the shortest path between consecutive nodes */
        std::vector<int> path = PathReconstruction(start, end, OptimizedRoute::s_shortestRouteMatrix);

        /* Add intermediate nodes, skipping the start node which is already included */
        for (size_t j = 1; j < path.size(); j++) {
            /* Only add nodes that haven't been visited yet */
            if (std::find(expandedRoute.begin(), expandedRoute.end(), path[j]) == expandedRoute.end()) {
                expandedRoute.push_back(path[j]);
            }
        }
    }

    return expandedRoute;
}

bool MSTRoute::CalculateRoute(const std::vector<WasteLocation>& locations) {
    /* Filter destinations based on waste threshold and distance */
    m_filteredDestinations = FilterDestinations(locations);

    /* Determine if waste pickup is needed */
    m_pickupRequired = !m_filteredDestinations.empty();

    /* No calculation needed if no pickup is required */
    if (!m_pickupRequired) {
        return false;
    }

    /* Build Minimum Spanning Tree from filtered destinations */
    std::vector<std::pair<int, int>> mst = BuildMST(m_filteredDestinations);

    /* Generate Euler tour starting from station (node 0) */
    std::vector<int> eulerTour = GenerateEulerTour(mst, 0);

    /* Convert Euler tour to Hamiltonian path by removing revisits */
    std::vector<int> shortcutTour = ShortcutEulerTour(eulerTour);

    /* Improve route by adding optimal intermediate nodes */
    m_finalRoute = ExpandRouteWithIntermediateNodes(shortcutTour);

    /* Calculate distances between consecutive locations in the route */
    m_individualDistances = CalculateSegmentDistances(m_finalRoute);

    /* Compute total route cost based on distances */
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
