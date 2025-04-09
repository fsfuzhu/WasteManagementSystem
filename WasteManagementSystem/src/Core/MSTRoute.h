// MSTRoute.h
// This file defines the MST (Minimum Spanning Tree) route algorithm
#pragma once

#include "Route.h"
#include <vector>
#include <unordered_set>
#include <utility>

/**
 * @brief Implementation of the Minimum Spanning Tree route algorithm
 *
 * This class implements a route optimization algorithm based on the
 * Minimum Spanning Tree algorithm (Prim's algorithm). It selects routes
 * that minimize the total distance while visiting all waste locations
 * that meet the waste threshold criteria (¡Ý 40%) and are within 15km
 * of the station.
 */
class MSTRoute : public Route {
private:
    /* Private members in MSTRoute class */
    std::vector<int> m_filteredDestinations;   // Locations that need collection
    const float m_maxDistanceFromStation = 15.0f; // Max distance from station (15km)
    bool m_pickupRequired = true;              // Whether any pickup is needed

    // Filter destinations by waste level and distance from station
    std::vector<int> FilterDestinations(const std::vector<WasteLocation>& locations);

    // Build MST using Prim's algorithm
    std::vector<std::pair<int, int>> BuildMST(const std::vector<int>& destinations);

    // Generate Euler tour of the MST
    std::vector<int> GenerateEulerTour(const std::vector<std::pair<int, int>>& mst, int startNode);

    // Shortcut the Euler tour to get a Hamiltonian path (remove duplicates)
    std::vector<int> ShortcutEulerTour(const std::vector<int>& eulerTour);

    // Create an adjacency list from MST edges
    std::vector<std::vector<int>> CreateAdjacencyList(
        const std::vector<std::pair<int, int>>& mst, int numNodes);

    // DFS traversal for Euler tour
    void DFSTraversal(int node, std::vector<std::vector<int>>& adjList,
        std::vector<int>& tour);

    // Calculate segment distances
    std::vector<float> CalculateSegmentDistances(const std::vector<int>& route);

public:
    /**
     * @brief Constructor for MSTRoute
     */
    MSTRoute();

    /**
     * @brief Destructor for MSTRoute
     */
    virtual ~MSTRoute();

    /**
     * @brief Calculate optimized MST route
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