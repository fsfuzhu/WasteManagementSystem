// TSPRoute.cpp
// Implementation of the TSP route algorithm
#include "pch.h"
#include "TSPRoute.h"

TSPRoute::TSPRoute()
    : Route("TSP Route", 40.0f) // 40% waste threshold for TSP route
{
}

TSPRoute::~TSPRoute()
{
}

std::vector<int> TSPRoute::FilterDestinations(const std::vector<WasteLocation>& locations)
{
    std::vector<int> filteredDestinations;

    // Add locations that meet the waste threshold and are within distance limit
    for (const auto& location : locations) {
        if (location.GetWasteLevel() >= m_wasteThreshold) {
            int id = WasteLocation::dict_Name_toId[location.GetLocationName()];

            // Check if within maximum distance from station
            if (id > 0 && WasteLocation::map_distance_matrix[0][id] <= m_maxDistanceFromStation) {
                filteredDestinations.push_back(id);
            }
        }
    }

    return filteredDestinations;
}

bool TSPRoute::CalculateRoute(const std::vector<WasteLocation>& locations)
{
    // Clear previous route data
    m_finalRoute.clear();
    m_individualDistances.clear();
    m_totalDistance = 0.0f;
    m_timeTaken = 0.0f;
    m_fuelConsumption = 0.0f;
    m_wage = 0.0f;
    m_totalCost = 0.0f;

    // Filter destinations based on waste level and distance
    m_filteredDestinations = FilterDestinations(locations);

    // Check if any locations need pickup
    if (m_filteredDestinations.empty()) {
        m_pickupRequired = false;
        return false;
    }

    m_pickupRequired = true;

    // Add depot (station) to destinations
    std::vector<int> tspDestinations = m_filteredDestinations;

    // First, use nearest neighbor heuristic to get an initial solution
    std::vector<int> initialRoute = SolveNearestNeighbor(tspDestinations);

    // Then, improve the solution using 2-opt local search
    std::vector<int> improvedRoute = Improve2Opt(initialRoute);

    // Expand route to include intermediate nodes
    m_finalRoute = ExpandRouteWithIntermediateNodes(improvedRoute);

    // Calculate individual segment distances
    m_individualDistances = CalculateSegmentDistances(m_finalRoute);

    // Calculate costs
    CalculateCosts();

    return true;
}

std::vector<int> TSPRoute::SolveNearestNeighbor(const std::vector<int>& destinations)
{
    // Always start from the depot (station)
    std::vector<int> route;
    route.push_back(0); // Station

    // Copy destinations to a mutable list
    std::vector<int> remaining = destinations;

    // Current position is the depot
    int current = 0;

    // Build the route by repeatedly finding the nearest unvisited location
    while (!remaining.empty()) {
        float minDistance = std::numeric_limits<float>::max();
        int nearestIndex = -1;

        // Find the nearest unvisited location
        for (size_t i = 0; i < remaining.size(); i++) {
            // Use Floyd-Warshall distances to find shortest paths
            float distance = OptimizedRoute::s_floydWarshallMatrix[current][remaining[i]];

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

    // Return to the depot
    route.push_back(0);

    return route;
}

std::vector<int> TSPRoute::Improve2Opt(const std::vector<int>& route)
{
    // Create a copy of the route to optimize
    std::vector<int> improvedRoute = route;

    // Calculate initial route distance
    float bestDistance = CalculateRouteDistance(improvedRoute);

    // Keep track of whether any improvements were made
    bool improved = true;

    // Iterate until no more improvements can be made
    while (improved) {
        improved = false;

        // Try all possible 2-opt swaps
        for (size_t i = 1; i < improvedRoute.size() - 2; i++) {
            for (size_t j = i + 1; j < improvedRoute.size() - 1; j++) {
                // Check if swapping edges (i-1,i) and (j,j+1) improves the route
                if (Is2OptImprovement(improvedRoute, i, j, bestDistance)) {
                    // Apply the 2-opt swap: reverse the order of cities between i and j
                    std::reverse(improvedRoute.begin() + i, improvedRoute.begin() + j + 1);

                    // Update best distance
                    bestDistance = CalculateRouteDistance(improvedRoute);

                    // Mark as improved
                    improved = true;
                }
            }
        }
    }

    return improvedRoute;
}

bool TSPRoute::Is2OptImprovement(const std::vector<int>& route, int i, int j, float currentDistance)
{
    // Calculate the change in distance if we were to apply the 2-opt swap
    int a = route[i - 1];
    int b = route[i];
    int c = route[j];
    int d = route[j + 1];

    // Current edges: (a,b) and (c,d)
    float currentEdgeDistance = OptimizedRoute::s_floydWarshallMatrix[a][b] +
        OptimizedRoute::s_floydWarshallMatrix[c][d];

    // After swap, new edges: (a,c) and (b,d)
    float newEdgeDistance = OptimizedRoute::s_floydWarshallMatrix[a][c] +
        OptimizedRoute::s_floydWarshallMatrix[b][d];

    // Return true if the swap reduces the distance
    return newEdgeDistance < currentEdgeDistance;
}

float TSPRoute::CalculateRouteDistance(const std::vector<int>& route)
{
    float totalDistance = 0.0f;

    // Sum the distances between consecutive locations using Floyd-Warshall matrix
    for (size_t i = 0; i < route.size() - 1; i++) {
        float distance = OptimizedRoute::s_floydWarshallMatrix[route[i]][route[i + 1]];
        totalDistance += distance;
    }

    return totalDistance;
}

std::vector<int> TSPRoute::ExpandRouteWithIntermediateNodes(const std::vector<int>& basicRoute)
{
    std::vector<int> expandedRoute;

    // Nothing to expand for empty routes
    if (basicRoute.empty()) {
        return expandedRoute;
    }

    // Add the first node
    expandedRoute.push_back(basicRoute[0]);

    // For each adjacent pair of nodes, add intermediate nodes if needed
    for (size_t i = 0; i < basicRoute.size() - 1; i++) {
        int from = basicRoute[i];
        int to = basicRoute[i + 1];

        // If there's no direct connection, find the shortest path using Floyd-Warshall
        if (WasteLocation::map_distance_matrix[from][to] >= INF) {
            // Use the path reconstruction to find intermediate nodes
            std::vector<int> intermediatePath = PathReconstruction(from, to, OptimizedRoute::s_shortestRouteMatrix);

            // Skip the first node as it's already in the route
            for (size_t j = 1; j < intermediatePath.size(); j++) {
                expandedRoute.push_back(intermediatePath[j]);
            }
        }
        else {
            // Direct connection, just add the destination
            expandedRoute.push_back(to);
        }
    }

    return expandedRoute;
}

std::vector<int> TSPRoute::PathReconstruction(int start, int end, const int matrix[8][8])
{
    std::vector<int> path;

    // Add start node
    path.push_back(start);

    // Follow the path from start to end
    while (start != end) {
        start = matrix[start][end];
        path.push_back(start);
    }

    return path;
}

std::vector<float> TSPRoute::CalculateSegmentDistances(const std::vector<int>& route)
{
    std::vector<float> distances;

    // Calculate distances between consecutive locations using direct matrix values
    for (size_t i = 0; i < route.size() - 1; i++) {
        int from = route[i];
        int to = route[i + 1];

        // Use actual distance from map matrix
        float distance = WasteLocation::map_distance_matrix[from][to];

        // Ensure we have a valid distance
        if (distance >= INF) {
            // This should never happen with the properly expanded route
            std::cerr << "Error: No direct path between locations " << from << " and " << to << std::endl;
            // Use a default value for safety
            distance = 0.0f;
        }

        distances.push_back(distance);
    }

    return distances;
}

const std::vector<int>& TSPRoute::GetFilteredDestinations() const
{
    return m_filteredDestinations;
}

bool TSPRoute::IsPickupRequired() const
{
    return m_pickupRequired;
}