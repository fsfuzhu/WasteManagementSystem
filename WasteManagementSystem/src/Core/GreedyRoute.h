// GreedyRoute.h
// This file defines the Greedy route algorithm
#pragma once

#include "Route.h"
#include "OptimizedRoute.h"
#include <vector>
#include <algorithm>
#include <limits>

/**
 * @brief Greedy route algorithm
 * Visits locations where waste level >= threshold (30%)
 * regardless of distance from station
 */
class GreedyRoute : public Route {
private:
    /* Private members in GreedyRoute class */
    std::vector<int> m_filteredDestinations;  // Locations that need collection
    bool m_pickupRequired;                   // Whether any pickup is needed

    // Filter destinations by waste level
    std::vector<int> FilterDestinationsByWasteLevel(const std::vector<WasteLocation>& locations);

    // Generate greedy route visiting all filtered destinations
    std::vector<int> GenerateGreedyRoute(const std::vector<int>& destinations);

    // Path reconstruction helper (similar to OptimizedRoute)
    std::vector<int> PathReconstruction(int start, int end, const int matrix[9][9]);

    // Expand route with intermediate nodes for nodes not directly connected
    std::vector<int> ExpandRouteWithIntermediateNodes(const std::vector<int>& basicRoute);

    // Calculate individual segment distances
    std::vector<float> CalculateSegmentDistances(const std::vector<int>& route);

public:
    /**
     * @brief Constructor for GreedyRoute
     */
    GreedyRoute();

    /**
     * @brief Destructor for GreedyRoute
     */
    virtual ~GreedyRoute();

    /**
     * @brief Calculate greedy route visiting locations with waste level >= threshold
     * @param locations Vector of waste locations
     * @return True if a valid route was found, false if no pickup needed
     */
    virtual bool CalculateRoute(const std::vector<WasteLocation>& locations) override;

    // Get filtered destinations that need pickup
    const std::vector<int>& GetFilteredDestinations() const;

    // Check if pickup is required
    bool IsPickupRequired() const;
};