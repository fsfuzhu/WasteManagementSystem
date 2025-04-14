// OptimizedRoute.cpp
// Implementation of the OptimizedRoute class
#include "pch.h"
#include "OptimizedRoute.h"

// Initialize static members
float OptimizedRoute::s_floydWarshallMatrix[9][9];
int OptimizedRoute::s_shortestRouteMatrix[9][9];

OptimizedRoute::OptimizedRoute()
    : Route("Optimized Route", 60.0f), // 60% waste threshold for optimized route
    m_pickupRequired(false)
{
}

OptimizedRoute::~OptimizedRoute()
{
}

void OptimizedRoute::InitializeFloydWarshall()
{
    // ���Ƴ�ʼ�������
    for (int i = 0; i < 9; i++) {  // ��Ϊ 9
        for (int j = 0; j < 9; j++) {  // ��Ϊ 9
            s_floydWarshallMatrix[i][j] = WasteLocation::map_distance_matrix[i][j];
            s_shortestRouteMatrix[i][j] = j;
        }
    }

    // Floyd-Warshall �㷨
    for (int k = 0; k < 9; k++) {  // ��Ϊ 9
        for (int i = 0; i < 9; i++) {  // ��Ϊ 9
            for (int j = 0; j < 9; j++) {  // ��Ϊ 9
                if (s_floydWarshallMatrix[i][k] + s_floydWarshallMatrix[k][j] < s_floydWarshallMatrix[i][j]) {
                    s_floydWarshallMatrix[i][j] = s_floydWarshallMatrix[i][k] + s_floydWarshallMatrix[k][j];
                    s_shortestRouteMatrix[i][j] = s_shortestRouteMatrix[i][k];
                }
            }
        }
    }
}

std::vector<int> OptimizedRoute::FilterDestinationsByWasteLevel(const std::vector<WasteLocation>& locations)
{
    std::vector<int> filteredDestinations;

    // Add station (starting point) first
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

std::vector<int> OptimizedRoute::GenerateFullRoute(
    const std::vector<int>& filteredDestinations,
    const float floydWarshallMatrix[9][9],
    const int shortestRouteMatrix[9][9])
{
    std::vector<int> finalRoute;
    // Make a copy of the filtered destinations without station (which will be handled separately)
    std::vector<int> remainingDestinations(filteredDestinations);

    // Start from the station
    int currentLocation = 0;
    finalRoute.push_back(currentLocation);

    // While there are still destinations to visit
    while (!remainingDestinations.empty()) {
        // Find nearest destination (greedy approach)
        int nearestIndex = 0;
        float minDistance = INF;
        for (size_t i = 0; i < remainingDestinations.size(); i++) {
            float distance = floydWarshallMatrix[currentLocation][remainingDestinations[i]];
            if (distance < minDistance) {
                minDistance = distance;
                nearestIndex = i;
            }
        }

        // Get the next destination
        int nextLocation = remainingDestinations[nearestIndex];

        // Find the shortest path to the next location using Floyd-Warshall
        std::vector<int> path = PathReconstruction(currentLocation, nextLocation, shortestRouteMatrix);

        // Add each node in the path (excluding the first one which is already in the route)
        for (size_t i = 1; i < path.size(); i++) {
            // Only add if not already in final route
            if (std::find(finalRoute.begin(), finalRoute.end(), path[i]) == finalRoute.end()) {
                finalRoute.push_back(path[i]);
            }
        }

        // Update current location
        currentLocation = nextLocation;

        // Remove all visited destinations from the remaining list
        for (size_t i = 0; i < finalRoute.size(); i++) {
            auto it = std::find(remainingDestinations.begin(), remainingDestinations.end(), finalRoute[i]);
            if (it != remainingDestinations.end()) {
                remainingDestinations.erase(it);
            }
        }
    }

    // Return to station if we visited any locations and not already there
    if (finalRoute.size() > 1 && finalRoute.back() != 0) {
        // Find path back to station
        std::vector<int> returnPath = PathReconstruction(finalRoute.back(), 0, shortestRouteMatrix);

        // Add each node in the return path (excluding the first one)
        for (size_t i = 1; i < returnPath.size(); i++) {
            // Only add if not already in final route (except station which can appear at beginning and end)
            if (returnPath[i] == 0 || std::find(finalRoute.begin(), finalRoute.end(), returnPath[i]) == finalRoute.end()) {
                finalRoute.push_back(returnPath[i]);
            }
        }
    }

    return finalRoute;
}
std::vector<int> OptimizedRoute::PathReconstruction(int start, int end, const int matrix[9][9])
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
bool OptimizedRoute::CalculateRoute(const std::vector<WasteLocation>& locations) {
    // ɸѡ��Ҫ���ʵ�Ŀ�ĵ�
    m_filteredDestinations = FilterDestinationsByWasteLevel(locations);

    // ����Ƿ���Ҫ�ռ�
    m_pickupRequired = !m_filteredDestinations.empty();

    if (!m_pickupRequired) {
        return false; // û����Ҫ�ռ��ĵ�
    }

    // ʹ��Floyd-Warshall�㷨��̰���㷨��������·��
    m_finalRoute = GenerateFullRoute(m_filteredDestinations, s_floydWarshallMatrix, s_shortestRouteMatrix);

    // ����ÿ�ξ���
    m_individualDistances.clear();
    for (size_t i = 0; i < m_finalRoute.size() - 1; i++) {
        m_individualDistances.push_back(WasteLocation::map_distance_matrix[m_finalRoute[i]][m_finalRoute[i + 1]]);
    }

    // �����ܾ��롢ʱ��ͳɱ�
    CalculateCosts();

    return true;
}
const std::vector<int>& OptimizedRoute::GetFilteredDestinations() const
{
    return m_filteredDestinations;
}

bool OptimizedRoute::IsPickupRequired() const
{
    return m_pickupRequired;
}