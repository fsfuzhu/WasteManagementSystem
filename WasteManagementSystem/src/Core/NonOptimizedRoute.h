// NonOptimizedRoute.h
// This file defines the non-optimized (regular) route that visits all locations
#pragma once

#include "Route.h"
#include <vector>
#include <algorithm>

/**
 * @brief Non-optimized route that visits locations with waste level >= 40%
 * and distance <= 30km from station
 *
 * This class implements a non-optimized route that visits all waste locations
 * that meet the waste level and distance criteria.
 */
class NonOptimizedRoute : public Route {
private:
    /* Private members in NonOptimizedRoute class */
    std::vector<int> m_defaultRoute;           // Default circular route order
    std::vector<int> m_filteredDestinations;   // Locations that need collection
    bool m_pickupRequired;                     // Whether any pickup is needed
    const float m_maxDistanceFromStation = 30.0f;  // Maximum distance from station (km)

    // Generate the default circular route
    std::vector<int> GenerateDefaultRoute();

    // Filter destinations by waste level and distance
    std::vector<int> FilterDestinations(const std::vector<WasteLocation>& locations);

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
     * @brief Calculate non-optimized route visiting locations with waste level >= 40%
     * and are within 30km from the station
     * @param locations Vector of waste locations
     * @return True if a valid route was found, false if no pickup needed
     */
    virtual bool CalculateRoute(const std::vector<WasteLocation>& locations) override;

    /**
     * @brief Get filtered destinations that need pickup
     */
    const std::vector<int>& GetFilteredDestinations() const;

    /**
     * @brief Check if pickup is required
     */
    bool IsPickupRequired() const;
};