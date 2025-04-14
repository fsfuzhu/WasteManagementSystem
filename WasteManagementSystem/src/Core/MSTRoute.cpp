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

            // ʹ��ֱ�߾������Ƿ��������뷶Χ��
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

            // ʹ��ֱ�߾���
            float weight = WasteLocation::map_distance_matrix[uId][vId];

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
        // Check if the parent is valid
        if (parent[i] != -1) {
            // Add edge between parent[i] and i using actual node IDs
            mst.push_back({ destinations[parent[i]], destinations[i] });
        }
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
    std::vector<int>& tour, std::vector<bool>& visited)
{
    visited[node] = true;
    tour.push_back(node);

    for (int neighbor : adjList[node]) {
        if (!visited[neighbor]) {
            DFSTraversal(neighbor, adjList, tour, visited);
        }
    }
}


std::vector<int> MSTRoute::GenerateEulerTour(const std::vector<std::pair<int, int>>& mst, int startNode)
{
    // �����ڽӱ�
    int numNodes = 8; // �ܹ���8��λ�� (0-7)
    std::vector<std::vector<int>> adjList = CreateAdjacencyList(mst, numNodes);

    // ����������ȱ���
    std::vector<int> tour;
    std::vector<bool> visited(numNodes, false);

    // ��ʼDFS����
    DFSTraversal(startNode, adjList, tour, visited);

    return tour;
}

std::vector<int> MSTRoute::ShortcutEulerTour(const std::vector<int>& eulerTour)
{
    std::vector<int> hamiltonianPath;
    std::unordered_set<int> visitedNodes;

    for (int node : eulerTour) {
        // ֻ���δ���ʹ��Ľڵ�
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

    // ����·����ÿ�����ڽڵ��֮��ľ���
    for (size_t i = 0; i < route.size() - 1; i++) {
        int from = route[i];
        int to = route[i + 1];
        float distance = WasteLocation::map_distance_matrix[from][to];
        distances.push_back(distance);
    }

    return distances;
}
std::vector<int> MSTRoute::PathReconstruction(int start, int end, const int matrix[8][8]) {
    std::vector<int> path;
    while (start != end) {
        path.push_back(start);
        start = matrix[start][end];
    }
    path.push_back(end);
    return path;
}
std::vector<int> MSTRoute::ExpandRouteWithIntermediateNodes(const std::vector<int>& basicRoute) {
    std::vector<int> expandedRoute;

    // ��ӵ�һ���ڵ�
    if (!basicRoute.empty()) {
        expandedRoute.push_back(basicRoute[0]);
    }

    // ��չ·���������ظ�
    for (size_t i = 0; i < basicRoute.size() - 1; i++) {
        int start = basicRoute[i];
        int end = basicRoute[i + 1];

        // �ҵ���start��end�����·��
        std::vector<int> path = PathReconstruction(start, end, OptimizedRoute::s_shortestRouteMatrix);

        // ����м�ڵ� (������㣬���Ѿ���expandedRoute��)
        for (size_t j = 1; j < path.size(); j++) {
            // ֻ���δ���ʹ��Ľڵ�
            if (std::find(expandedRoute.begin(), expandedRoute.end(), path[j]) == expandedRoute.end()) {
                expandedRoute.push_back(path[j]);
            }
        }
    }

    return expandedRoute;
}
bool MSTRoute::CalculateRoute(const std::vector<WasteLocation>& locations) {
    // ɸѡ��Ҫ���ʵ�Ŀ�ĵ�
    m_filteredDestinations = FilterDestinations(locations);

    // ����Ƿ���Ҫ�ռ�
    m_pickupRequired = !m_filteredDestinations.empty();

    if (!m_pickupRequired) {
        return false; // û����Ҫ�ռ��ĵ�
    }

    // ʹ��Prim�㷨����MST
    std::vector<std::pair<int, int>> mst = BuildMST(m_filteredDestinations);

    // ����ŷ����·
    std::vector<int> eulerTour = GenerateEulerTour(mst, 0); // ��վ��0��ʼ

    // ��ŷ����·ת��Ϊ���ܶ�·��
    std::vector<int> shortcutTour = ShortcutEulerTour(eulerTour);

    // ��չ·���԰����м�ڵ� - ������һ��
    // ע�⣺MST����Ҫ�������
    m_finalRoute = ExpandRouteWithIntermediateNodes(shortcutTour);

    // ����ÿ�ξ���
    m_individualDistances = CalculateSegmentDistances(m_finalRoute);

    // �����ܾ��롢ʱ��ͳɱ�
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