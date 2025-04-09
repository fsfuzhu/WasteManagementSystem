// NonOptimizedRoute.h
// This file defines the non-optimized (regular) route that visits all locations
#pragma once

#include "Route.h"
#include <vector>

/**
 * @brief Non-optimized route that visits all waste locations
 *
 * This class implements a non-optimized route that visits all waste locations,
 * regardless of their waste levels. This represents the default circular route
 * where the truck drives directly to each location and returns to the station.
 */
class NonOptimizedRoute : public Route {
private:
    /* Private members in NonOptimizedRoute class */
    std::vector<int> m_defaultRoute;  // Default circular route to visit all locations

    // Generate the default circular route
    std::vector<int> GenerateDefaultRoute();

    // Calculate individual segment distances
    std::vector<float> CalculateSegmentDistances(const std::vector<int>& route);

public:
    /**
     * @brief Constructor for NonOptimizedRoute
     */
    NonOptimizedRoute();

    /**
     * @brief Destructor for NonOptimizedRoute
     */
    virtual ~NonOptimizedRoute();

    /**
     * @brief Calculate non-optimized route visiting all locations
     * @param locations Vector of waste locations
     * @return Always returns true (default route always exists)
     */
    virtual bool CalculateRoute(const std::vector<WasteLocation>& locations) override;
};