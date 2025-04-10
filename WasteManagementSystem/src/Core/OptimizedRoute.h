// OptimizedRoute.h
// This file defines the optimized route algorithm using Floyd-Warshall algorithm
#pragma once

#include "Route.h"
#include <algorithm>
#include <limits>

/**
 * @brief Optimized route using Floyd-Warshall algorithm
 * Only visits locations where waste level >= threshold (60%)
 */
class OptimizedRoute : public Route {
private:
    /* Private members in OptimizedRoute class */
    std::vector<int> m_filteredDestinations;   // Locations that need collection
    bool m_pickupRequired;                     // Whether any pickup is needed

    // Filter destinations by waste level
    std::vector<int> FilterDestinationsByWasteLevel(const std::vector<WasteLocation>& locations);

    // Reconstruct shortest path between two nodes
    std::vector<int> PathReconstruction(int start, int end, const int matrix[8][8]);

    // Generate full route visiting all filtered destinations
    std::vector<int> GenerateFullRoute(
        const std::vector<int>& filteredDestinations,
        const float floydWarshallMatrix[8][8],
        const int shortestRouteMatrix[8][8]);

public:
    // Made these public so they can be accessed by other route classes
    static float s_floydWarshallMatrix[8][8];  // Shortest path distances
    static int s_shortestRouteMatrix[8][8];    // Next node in shortest path

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

    /**
     * @brief Calculate optimized route visiting locations with waste level >= threshold
     * @param locations Vector of waste locations
     * @return True if a valid route was found, false if no pickup needed
     */
    virtual bool CalculateRoute(const std::vector<WasteLocation>& locations) override;

    // Get filtered destinations that need pickup
    const std::vector<int>& GetFilteredDestinations() const;

    // Check if pickup is required
    bool IsPickupRequired() const;
};