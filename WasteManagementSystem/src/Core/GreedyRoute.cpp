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

        // Find the nearest unvisited location
        for (size_t i = 0; i < remaining.size(); i++) {
            float distance = WasteLocation::map_distance_matrix[current][remaining[i]];

            // If no direct path, use a large value
            if (distance >= INF) {
                distance = 1000.0f; // Large but finite value
            }

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

std::vector<float> GreedyRoute::CalculateSegmentDistances(const std::vector<int>& route)
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
    m_finalRoute = GenerateGreedyRoute(m_filteredDestinations);

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