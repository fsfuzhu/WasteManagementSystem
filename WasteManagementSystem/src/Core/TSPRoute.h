// TSPRoute.h
// This file defines the TSP route algorithm
#pragma once

#include "Route.h"
#include "OptimizedRoute.h"
#include <vector>
#include <algorithm>
#include <limits>

/**
 * @brief TSP route using Nearest Neighbor and 2-opt optimization
 * Only visits locations where waste level >= threshold (40%)
 * and are within 15km from the station
 * Must return to the starting point (closed loop)
 */
class TSPRoute : public Route {
private:
    /* Private members in TSPRoute class */
    std::vector<int> m_filteredDestinations;  // Locations that need collection
    bool m_pickupRequired;                   // Whether any pickup is needed
    const float m_maxDistanceFromStation = 15.0f;  // Maximum distance from station (km)

    // Filter destinations by waste level and distance from station
    std::vector<int> FilterDestinations(const std::vector<WasteLocation>& locations);

    // Use Nearest Neighbor algorithm to get initial TSP solution
    std::vector<int> SolveNearestNeighbor(const std::vector<int>& destinations);

    // Use 2-opt local search to improve the route
    std::vector<int> Improve2Opt(const std::vector<int>& route);

    // Check if a 2-opt swap would improve the route
    bool Is2OptImprovement(const std::vector<int>& route, int i, int j, float currentDistance);

    // Calculate total distance of a route
    float CalculateRouteDistance(const std::vector<int>& route);

    // Expand route with intermediate nodes for nodes not directly connected
    std::vector<int> ExpandRouteWithIntermediateNodes(const std::vector<int>& basicRoute);

    // Path reconstruction helper (similar to OptimizedRoute)
    std::vector<int> PathReconstruction(int start, int end, const int matrix[9][9]);

    // Calculate individual segment distances
    std::vector<float> CalculateSegmentDistances(const std::vector<int>& route);

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
     * @brief Calculate TSP route visiting locations with waste level >= threshold
     * @param locations Vector of waste locations
     * @return True if a valid route was found, false if no pickup needed
     */
    virtual bool CalculateRoute(const std::vector<WasteLocation>& locations) override;


    // Get filtered destinations that need pickup
    const std::vector<int>& GetFilteredDestinations() const;

    // Check if pickup is required
    bool IsPickupRequired() const;
};