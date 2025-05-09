// OptimizedRoute.h
// This file defines the optimized route algorithm using Floyd-Warshall algorithm
#pragma once

#include "Route.h"
#include <algorithm>
#include <limits>

/**
 * @brief Optimized route using Floyd-Warshall algorithm
 * Only visits locations where waste level >= threshold (60%)
 * and distance <= 20km from station
 */
class OptimizedRoute : public Route {
private:
    /* Private members in OptimizedRoute class */
    std::vector<int> m_filteredDestinations;   // Locations that need collection
    bool m_pickupRequired;                     // Whether any pickup is needed
    const float m_maxDistanceFromStation = 20.0f;  // Maximum distance from station (km)

    // Filter destinations by waste level and distance
    std::vector<int> FilterDestinationsByWasteLevel(const std::vector<WasteLocation>& locations);

    // Generate full route visiting all filtered destinations
    std::vector<int> GenerateFullRoute(
        const std::vector<int>& filteredDestinations,
        const float floydWarshallMatrix[9][9],
        const int shortestRouteMatrix[9][9]);

public:
    // Made these public so they can be accessed by other route classes
    static float s_floydWarshallMatrix[9][9];  // Shortest path distances
    static int s_shortestRouteMatrix[9][9];    // Next node in shortest path

    /**
     * @brief Constructor for OptimizedRoute
     * @param wasteThreshold Minimum waste level for collection (%)
     */
    OptimizedRoute();
    virtual ~OptimizedRoute();

    /**
     * @brief Initialize Floyd-Warshall algorithm matrices
     * Must be called once before using OptimizedRoute
     */
    static void InitializeFloydWarshall();
    std::vector<int> PathReconstruction(int start, int end, const int matrix[9][9]);
    /**
     * @brief Calculate optimized route visiting locations with waste level >= threshold
     * and are within 20km from the station
     * @param locations Vector of waste locations
     * @return True if a valid route was found, false if no pickup needed
     */
    virtual bool CalculateRoute(const std::vector<WasteLocation>& locations) override;

    // Get filtered destinations that need pickup
    const std::vector<int>& GetFilteredDestinations() const;

    // Check if pickup is required
    bool IsPickupRequired() const;
};