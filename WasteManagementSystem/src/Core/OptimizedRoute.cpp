// OptimizedRoute.cpp
// Implementation of the OptimizedRoute class
#include "pch.h"
#include "OptimizedRoute.h"

// Initialize static members
float OptimizedRoute::s_floydWarshallMatrix[8][8];
int OptimizedRoute::s_shortestRouteMatrix[8][8];

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
    // ������������ʹ��ֱ�߾��룬���е�֮�䶼����ֱ������
    // Floyd-Warshall�㷨������Ҫ����Ѱ���м�ڵ�
    // ��Ϊ�˱��ִ�������ԣ�������Ȼ���ƾ������s_floydWarshallMatrix

    // Copy distance matrix to floyd warshall matrix
    memcpy(&s_floydWarshallMatrix, &WasteLocation::map_distance_matrix,
        sizeof(WasteLocation::map_distance_matrix));

    // Initialize shortest route matrix
    // ��ֱ�����ӵ�����£����·������ֱ�Ӵ�һ�㵽��һ��
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            s_shortestRouteMatrix[i][j] = j;
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
    const float floydWarshallMatrix[8][8],
    const int shortestRouteMatrix[8][8])
{
    std::vector<int> finalRoute;

    // Make a copy of the filtered destinations without station (which will be handled separately)
    std::vector<int> remainingDestinations(filteredDestinations.begin() + 1, filteredDestinations.end());

    // Start from the station
    int currentLocation = 0;
    finalRoute.push_back(currentLocation);

    // While there are still destinations to visit
    while (!remainingDestinations.empty()) {
        // Find nearest destination (greedy approach)
        int nearestIndex = 0;
        float minDistance = INF;

        for (size_t i = 0; i < remainingDestinations.size(); i++) {
            float distance = WasteLocation::map_distance_matrix[currentLocation][remainingDestinations[i]];
            if (distance < minDistance) {
                minDistance = distance;
                nearestIndex = i;
            }
        }

        // Get the next destination
        int nextLocation = remainingDestinations[nearestIndex];

        // ��ֱ�����ӵ�����£�·������ֱ�Ӵӵ�ǰλ�õ���һ��λ��
        finalRoute.push_back(nextLocation);

        // Update current location
        currentLocation = nextLocation;

        // Remove visited destination
        remainingDestinations.erase(remainingDestinations.begin() + nearestIndex);
    }

    // Return to station if we visited any locations (for TSP-like routes)
    // ������Ը�����Ҫ�����Ƿ���Ҫ�������
    if (finalRoute.size() > 1) {
        finalRoute.push_back(0); // Return to station
    }

    return finalRoute;
}

bool OptimizedRoute::CalculateRoute(const std::vector<WasteLocation>& locations)
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
    m_filteredDestinations = FilterDestinationsByWasteLevel(locations);

    // Check if any locations need pickup (besides the station)
    if (m_filteredDestinations.size() <= 1) {
        m_pickupRequired = false;
        return false;
    }

    m_pickupRequired = true;

    // Generate the optimized route
    m_finalRoute = GenerateFullRoute(m_filteredDestinations, s_floydWarshallMatrix, s_shortestRouteMatrix);

    // Calculate distances between consecutive locations
    for (size_t i = 0; i < m_finalRoute.size() - 1; i++) {
        int fromId = m_finalRoute[i];
        int toId = m_finalRoute[i + 1];

        // Get distance from distance matrix
        float distance = WasteLocation::map_distance_matrix[fromId][toId];
        m_individualDistances.push_back(distance);
    }

    // Calculate costs
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