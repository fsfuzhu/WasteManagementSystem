// GreedyRoute.cpp
// Implementation of the Greedy route algorithm
#include "pch.h"
#include "GreedyRoute.h"

GreedyRoute::GreedyRoute()
    : Route("Greedy Route", 30.0f) // 30% waste threshold for greedy route
{
}

GreedyRoute::~GreedyRoute()
{
}

std::vector<int> GreedyRoute::FilterDestinationsByWasteLevel(const std::vector<WasteLocation>& locations)
{
    std::vector<int> filteredDestinations;

    // Add locations that meet the waste threshold
    for (const auto& location : locations) {
        if (location.GetWasteLevel() >= m_wasteThreshold) {
            int id = WasteLocation::dict_Name_toId[location.GetLocationName()];
            if (id > 0) { // Skip station (0)
                filteredDestinations.push_back(id);
            }
        }
    }

    return filteredDestinations;
}

std::vector<int> GreedyRoute::GenerateGreedyRoute(const std::vector<int>& destinations)
{
    // Always start from the station
    std::vector<int> route;
    route.push_back(0); // Station

    // Copy destinations to a mutable list
    std::vector<int> remaining = destinations;

    // Current position is the station
    int current = 0;

    // Build the route by repeatedly finding the nearest unvisited location
    while (!remaining.empty()) {
        float minDistance = std::numeric_limits<float>::max();
        int nearestIndex = -1;

        // Find the nearest unvisited location using Floyd-Warshall shortest paths
        for (size_t i = 0; i < remaining.size(); i++) {
            float distance = OptimizedRoute::s_floydWarshallMatrix[current][remaining[i]];

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

    // Return to the station
    route.push_back(0);

    return route;
}

std::vector<int> GreedyRoute::ExpandRouteWithIntermediateNodes(const std::vector<int>& basicRoute)
{
    std::vector<int> expandedRoute;

    // Nothing to expand for empty routes
    if (basicRoute.empty()) {
        return expandedRoute;
    }

    // Add the first node
    expandedRoute.push_back(basicRoute[0]);

    // For each adjacent pair of nodes, add intermediate nodes if needed
    for (size_t i = 0; i < basicRoute.size() - 1; i++) {
        int from = basicRoute[i];
        int to = basicRoute[i + 1];

        // If there's no direct connection, find the shortest path using Floyd-Warshall
        if (WasteLocation::map_distance_matrix[from][to] >= INF) {
            // Use the path reconstruction to find intermediate nodes
            std::vector<int> intermediatePath = PathReconstruction(from, to, OptimizedRoute::s_shortestRouteMatrix);

            // Skip the first node as it's already in the route
            for (size_t j = 1; j < intermediatePath.size(); j++) {
                expandedRoute.push_back(intermediatePath[j]);
            }
        }
        else {
            // Direct connection, just add the destination
            expandedRoute.push_back(to);
        }
    }

    return expandedRoute;
}

std::vector<int> GreedyRoute::PathReconstruction(int start, int end, const int matrix[8][8])
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

std::vector<float> GreedyRoute::CalculateSegmentDistances(const std::vector<int>& route)
{
    std::vector<float> distances;

    // Calculate distances between consecutive locations using direct map distances
    for (size_t i = 0; i < route.size() - 1; i++) {
        int from = route[i];
        int to = route[i + 1];

        // Use actual distance from map matrix
        float distance = WasteLocation::map_distance_matrix[from][to];

        // Ensure we have a valid distance
        if (distance >= INF) {
            // This should never happen with the properly expanded route
            std::cerr << "Error: No direct path between locations " << from << " and " << to << std::endl;
            // Use a default value for safety
            distance = 0.0f;
        }

        distances.push_back(distance);
    }

    return distances;
}

bool GreedyRoute::CalculateRoute(const std::vector<WasteLocation>& locations)
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

    // Check if any locations need pickup
    if (m_filteredDestinations.empty()) {
        m_pickupRequired = false;
        return false;
    }

    m_pickupRequired = true;

    // Generate the greedy route
    std::vector<int> basicRoute = GenerateGreedyRoute(m_filteredDestinations);

    // Expand route with intermediate nodes for locations not directly connected
    m_finalRoute = ExpandRouteWithIntermediateNodes(basicRoute);

    // Calculate individual segment distances
    m_individualDistances = CalculateSegmentDistances(m_finalRoute);

    // Calculate costs
    CalculateCosts();

    return true;
}

const std::vector<int>& GreedyRoute::GetFilteredDestinations() const
{
    return m_filteredDestinations;
}

bool GreedyRoute::IsPickupRequired() const
{
    return m_pickupRequired;
}