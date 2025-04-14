// TSPRoute.cpp
// Implementation of the TSP route algorithm
#include "pch.h"
#include "TSPRoute.h"

TSPRoute::TSPRoute()
    : Route("TSP Route", 40.0f) // 40% waste threshold for TSP route
{
}

TSPRoute::~TSPRoute()
{
}
std::vector<int> TSPRoute::PathReconstruction(int start, int end, const int matrix[9][9])
{
    std::vector<int> path;
    while (start != end)
    {
        path.push_back(start);
        start = matrix[start][end];
    }
    path.push_back(end);
    return path;
}
std::vector<float> TSPRoute::CalculateSegmentDistances(const std::vector<int>& route)
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
std::vector<int> TSPRoute::ExpandRouteWithIntermediateNodes(const std::vector<int>& basicRoute)
{
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
            // ֻ���δ���ʹ��Ľڵ㣬�������һ���ڵ������Station(0)
            if (std::find(expandedRoute.begin(), expandedRoute.end(), path[j]) == expandedRoute.end() ||
                (path[j] == 0 && i == basicRoute.size() - 2)) { // ������Station
                expandedRoute.push_back(path[j]);
            }
        }
    }

    return expandedRoute;
}
std::vector<int> TSPRoute::FilterDestinations(const std::vector<WasteLocation>& locations)
{
    std::vector<int> filteredDestinations;

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

bool TSPRoute::CalculateRoute(const std::vector<WasteLocation>& locations) {
    // ɸѡ��Ҫ���ʵ�Ŀ�ĵ�
    m_filteredDestinations = FilterDestinations(locations);

    // ����Ƿ���Ҫ�ռ�
    m_pickupRequired = !m_filteredDestinations.empty();

    if (!m_pickupRequired) {
        return false; // û����Ҫ�ռ��ĵ�
    }

    // ʹ��������㷨��ȡ��ʼTSP�������
    std::vector<int> initialRoute = SolveNearestNeighbor(m_filteredDestinations);

    // ʹ��2-opt���������Ľ�·��
    std::vector<int> improvedRoute = Improve2Opt(initialRoute);

    // ��չ·���԰����м�ڵ� - ������һ��
    m_finalRoute = ExpandRouteWithIntermediateNodes(improvedRoute);

    // ����ÿ�ξ���
    m_individualDistances = CalculateSegmentDistances(m_finalRoute);

    // �����ܾ��롢ʱ��ͳɱ�
    CalculateCosts();

    return true;
}

std::vector<int> TSPRoute::SolveNearestNeighbor(const std::vector<int>& destinations)
{
    // Always start from the depot (station)
    std::vector<int> route;
    route.push_back(0); // Station

    // Copy destinations to a mutable list
    std::vector<int> remaining = destinations;

    // Current position is the depot
    int current = 0;

    // Build the route by repeatedly finding the nearest unvisited location
    while (!remaining.empty()) {
        float minDistance = std::numeric_limits<float>::max();
        int nearestIndex = -1;

        // Find the nearest unvisited location
        for (size_t i = 0; i < remaining.size(); i++) {
            // ʹ��ֱ�߾���
            float distance = WasteLocation::map_distance_matrix[current][remaining[i]];

            if (distance < minDistance) {
                minDistance = distance;
                nearestIndex = i;
            }
        }

        // Move to the nearest location
        if (nearestIndex >= 0) {
            current = remaining[nearestIndex];
            route.push_back(current);

            // Remove the visited location
            remaining.erase(remaining.begin() + nearestIndex);
        }
        else {
            // No reachable locations left
            break;
        }
    }

    // Return to the depot (TSPҪ��ص����)
    route.push_back(0);

    return route;
}

std::vector<int> TSPRoute::Improve2Opt(const std::vector<int>& route)
{
    // Create a copy of the route to optimize
    std::vector<int> improvedRoute = route;

    // Calculate initial route distance
    float bestDistance = CalculateRouteDistance(improvedRoute);

    // Keep track of whether any improvements were made
    bool improved = true;

    // Iterate until no more improvements can be made
    while (improved) {
        improved = false;

        // Try all possible 2-opt swaps
        for (size_t i = 1; i < improvedRoute.size() - 2; i++) {
            for (size_t j = i + 1; j < improvedRoute.size() - 1; j++) {
                // Check if swapping edges (i-1,i) and (j,j+1) improves the route
                if (Is2OptImprovement(improvedRoute, i, j, bestDistance)) {
                    // Apply the 2-opt swap: reverse the order of cities between i and j
                    std::reverse(improvedRoute.begin() + i, improvedRoute.begin() + j + 1);

                    // Update best distance
                    bestDistance = CalculateRouteDistance(improvedRoute);

                    // Mark as improved
                    improved = true;
                }
            }
        }
    }

    return improvedRoute;
}

bool TSPRoute::Is2OptImprovement(const std::vector<int>& route, int i, int j, float currentDistance)
{
    // Calculate the change in distance if we were to apply the 2-opt swap
    int a = route[i - 1];
    int b = route[i];
    int c = route[j];
    int d = route[j + 1];

    // Current edges: (a,b) and (c,d)
    float currentEdgeDistance = WasteLocation::map_distance_matrix[a][b] +
        WasteLocation::map_distance_matrix[c][d];

    // After swap, new edges: (a,c) and (b,d)
    float newEdgeDistance = WasteLocation::map_distance_matrix[a][c] +
        WasteLocation::map_distance_matrix[b][d];

    // Return true if the swap reduces the distance
    return newEdgeDistance < currentEdgeDistance;
}

float TSPRoute::CalculateRouteDistance(const std::vector<int>& route)
{
    float totalDistance = 0.0f;

    // Sum the distances between consecutive locations
    for (size_t i = 0; i < route.size() - 1; i++) {
        float distance = WasteLocation::map_distance_matrix[route[i]][route[i + 1]];
        totalDistance += distance;
    }

    return totalDistance;
}

const std::vector<int>& TSPRoute::GetFilteredDestinations() const
{
    return m_filteredDestinations;
}

bool TSPRoute::IsPickupRequired() const
{
    return m_pickupRequired;
}