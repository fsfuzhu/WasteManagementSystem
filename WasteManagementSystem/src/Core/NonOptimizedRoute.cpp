// NonOptimizedRoute.cpp
// Implementation of the non-optimized route algorithm
#include "pch.h"
#include "NonOptimizedRoute.h"

NonOptimizedRoute::NonOptimizedRoute()
    : Route("Non-Optimized Route", 0.0f) // No waste threshold for non-optimized route
{
    // Initialize default route
    m_defaultRoute = GenerateDefaultRoute();
}

NonOptimizedRoute::~NonOptimizedRoute()
{
}

std::vector<int> NonOptimizedRoute::GenerateDefaultRoute()
{
    // Create a circular route that visits all locations in order
    // Starting and ending at the station (0)
    std::vector<int> route = { 0, 1, 3, 2, 4, 3, 6, 5, 6, 7, 0 };

    // This specific route was defined in the course sample code
    // It follows the structure:
    // Station -> A -> C -> B -> D -> C -> F -> E -> F -> G -> Station

    return route;
}

std::vector<float> NonOptimizedRoute::CalculateSegmentDistances(const std::vector<int>& route)
{
    std::vector<float> distances;

    // Calculate distances between consecutive locations
    for (size_t i = 0; i < route.size() - 1; i++) {
        float distance = WasteLocation::map_distance_matrix[route[i]][route[i + 1]];

        // If no direct path, use a large value (shouldn't happen in the default route)
        if (distance >= INF) {
            distance = 1000.0f; // Large but finite value
        }

        distances.push_back(distance);
    }

    return distances;
}

bool NonOptimizedRoute::CalculateRoute(const std::vector<WasteLocation>& locations)
{
    // Clear previous route data
    m_finalRoute.clear();
    m_individualDistances.clear();
    m_totalDistance = 0.0f;
    m_timeTaken = 0.0f;
    m_fuelConsumption = 0.0f;
    m_wage = 0.0f;
    m_totalCost = 0.0f;

    // For non-optimized route, we always use the default route
    m_finalRoute = m_defaultRoute;

    // Calculate individual segment distances
    m_individualDistances = CalculateSegmentDistances(m_finalRoute);

    // Calculate costs
    CalculateCosts();

    return true; // Non-optimized route always exists
}