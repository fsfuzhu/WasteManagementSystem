// TSPRoute.h
// This file defines the TSP (Traveling Salesman Problem) route algorithm
#pragma once

#include "Route.h"
#include <vector>
#include <algorithm>
#include <limits>
#include <cstdint>

/**
 * @brief Implementation of the Traveling Salesman Problem route algorithm
 *
 * This class implements a route optimization algorithm based on the
 * Traveling Salesman Problem. It ensures the route starts and ends
 * at the Station (depot) while visiting all waste locations that meet
 * the waste threshold criteria (¡Ý 40%) and are within 15km of the station.
 */
class TSPRoute : public Route {
private:
    /* Private members in TSPRoute class */
    std::vector<int> m_filteredDestinations;   // Locations that need collection
    const float m_maxDistanceFromStation = 15.0f; // Max distance from station (15km)
    bool m_pickupRequired = true;              // Whether any pickup is needed

    // Filter destinations by waste level and distance from station
    std::vector<int> FilterDestinations(const std::vector<WasteLocation>& locations);

    // Solve the TSP using nearest neighbor heuristic
    std::vector<int> SolveNearestNeighbor(const std::vector<int>& destinations);

    // Solve the TSP using 2-opt local search improvement
    std::vector<int> Improve2Opt(const std::vector<int>& route);

    // Calculate route distance
    float CalculateRouteDistance(const std::vector<int>& route);

    // Calculate individual segment distances
    std::vector<float> CalculateSegmentDistances(const std::vector<int>& route);

    // Check if a 2-opt swap would improve the route
    bool Is2OptImprovement(const std::vector<int>& route, int i, int j, float currentDistance);

public:
    /**
     * @brief Constructor for TSPRoute
     */
    TSPRoute();

    /**
     * @brief Destructor for TSPRoute
     */
    virtual ~TSPRoute();

    /**
     * @brief Calculate optimized TSP route
     * @param locations Vector of waste locations
     * @return True if a valid route was found, false if no pickup needed
     */
    virtual bool CalculateRoute(const std::vector<WasteLocation>& locations) override;

    /**
     * @brief Get filtered destinations that need pickup
     * @return Vector of destination IDs
     */
    const std::vector<int>& GetFilteredDestinations() const;

    /**
     * @brief Check if pickup is required
     * @return True if pickup is required, false otherwise
     */
    bool IsPickupRequired() const;
};