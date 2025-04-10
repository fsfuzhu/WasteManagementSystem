// MSTRoute.h
// This file defines the MST route algorithm
#pragma once

#include "Route.h"
#include "OptimizedRoute.h"
#include <vector>
#include <utility>
#include <unordered_set>

/**
 * @brief MST route using Prim's algorithm
 * Only visits locations where waste level >= threshold (40%)
 * and are within 15km from the station
 */
class MSTRoute : public Route {
private:
    /* Private members in MSTRoute class */
    std::vector<int> m_filteredDestinations;  // Locations that need collection
    bool m_pickupRequired;                   // Whether any pickup is needed
    const float m_maxDistanceFromStation = 15.0f;  // Maximum distance from station (km)

    // Filter destinations by waste level and distance from station
    std::vector<int> FilterDestinations(const std::vector<WasteLocation>& locations);

    // Build MST using Prim's algorithm
    std::vector<std::pair<int, int>> BuildMST(const std::vector<int>& destinations);

    // Create adjacency list from MST edges
    std::vector<std::vector<int>> CreateAdjacencyList(
        const std::vector<std::pair<int, int>>& mst, int numNodes);

    // DFS traversal for Euler tour
    void DFSTraversal(int node, std::vector<std::vector<int>>& adjList,
        std::vector<int>& tour);

    // Generate Euler tour from MST
    std::vector<int> GenerateEulerTour(const std::vector<std::pair<int, int>>& mst, int startNode);

    // Shortcut Euler tour to get Hamiltonian path
    std::vector<int> ShortcutEulerTour(const std::vector<int>& eulerTour);

    // Expand route with intermediate nodes for nodes not directly connected
    std::vector<int> ExpandRouteWithIntermediateNodes(const std::vector<int>& shortcutTour);

    // Path reconstruction helper (similar to OptimizedRoute)
    std::vector<int> PathReconstruction(int start, int end, const int matrix[8][8]);

    // Calculate individual segment distances
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
     * @brief Calculate MST route visiting locations with waste level >= threshold
     * @param locations Vector of waste locations
     * @return True if a valid route was found, false if no pickup needed
     */
    virtual bool CalculateRoute(const std::vector<WasteLocation>& locations) override;

    // Get filtered destinations that need pickup
    const std::vector<int>& GetFilteredDestinations() const;

    // Check if pickup is required
    bool IsPickupRequired() const;
};