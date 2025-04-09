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
    // Initialize shortest route matrix
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            s_shortestRouteMatrix[i][j] = j;
        }
    }

    // Copy distance matrix to floyd warshall matrix
    memcpy(&s_floydWarshallMatrix, &WasteLocation::map_distance_matrix,
        sizeof(WasteLocation::map_distance_matrix));

    // Run Floyd-Warshall algorithm to find shortest paths between all pairs
    for (int k = 0; k < 8; k++) {
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                // If path through k is shorter than current path
                if (s_floydWarshallMatrix[i][j] >
                    s_floydWarshallMatrix[i][k] + s_floydWarshallMatrix[k][j]) {

                    // Update distance
                    s_floydWarshallMatrix[i][j] =
                        s_floydWarshallMatrix[i][k] + s_floydWarshallMatrix[k][j];

                    // Update next node in shortest path
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

    // Add locations that meet the waste threshold
    for (const auto& location : locations) {
        if (location.GetWasteLevel() >= m_wasteThreshold) {
            int id = WasteLocation::dict_Name_toId[location.GetLocationName()];
            filteredDestinations.push_back(id);
        }
    }

    return filteredDestinations;
}

std::vector<int> OptimizedRoute::PathReconstruction(int start, int end, const int matrix[8][8])
{
    std::vector<int> path;

    // Add start node
    path.push_back(start);

    // Follow the path from start to end
    while (start != end) {
        start = matrix[start][end];
        path.push_back(start);
    }

    return path;
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
            float distance = floydWarshallMatrix[currentLocation][remainingDestinations[i]];
            if (distance < minDistance) {
                minDistance = distance;
                nearestIndex = i;
            }
        }

        // Get the next destination
        int nextLocation = remainingDestinations[nearestIndex];

        // Get path from current location to next location
        std::vector<int> subPath = PathReconstruction(currentLocation, nextLocation, shortestRouteMatrix);

        // Skip the first node as it's already in the route
        for (size_t i = 1; i < subPath.size(); i++) {
            finalRoute.push_back(subPath[i]);

            // If this intermediate location is also in our remaining destinations, remove it
            auto it = std::find(remainingDestinations.begin(), remainingDestinations.end(), subPath[i]);
            if (it != remainingDestinations.end() && *it != nextLocation) {
                remainingDestinations.erase(it);
            }
        }

        // Update current location
        currentLocation = nextLocation;

        // Remove visited destination
        remainingDestinations.erase(remainingDestinations.begin() + nearestIndex);
    }

    // Return to station if we visited any locations
    if (finalRoute.size() > 1) {
        std::vector<int> returnPath = PathReconstruction(currentLocation, 0, shortestRouteMatrix);
        // Skip the first node as it's already in the route
        for (size_t i = 1; i < returnPath.size(); i++) {
            finalRoute.push_back(returnPath[i]);
        }
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

    // Filter destinations based on waste level
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

        // Get distance from floyd warshall matrix
        float distance = s_floydWarshallMatrix[fromId][toId];
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