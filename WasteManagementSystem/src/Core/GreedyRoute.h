// GreedyRoute.h
// This file defines the Greedy route algorithm
#pragma once

#include "Route.h"
#include <vector>

/**
 * @brief Implementation of the Greedy route algorithm
 *
 * This class implements a greedy route optimization algorithm that
 * visits waste locations with waste level ¡Ý 30%, regardless of the
 * distance from the station. It always chooses the nearest unvisited
 * location as the next destination.
 */
class GreedyRoute : public Route {
private:
    /* Private members in GreedyRoute class */
    std::vector<int> m_filteredDestinations;   // Locations that need collection
    bool m_pickupRequired = true;              // Whether any pickup is needed

    // Filter destinations by waste level
    std::vector<int> FilterDestinationsByWasteLevel(const std::vector<WasteLocation>& locations);

    // Generate a greedy route by always choosing the nearest unvisited location
    std::vector<int> GenerateGreedyRoute(const std::vector<int>& destinations);

    // Calculate segment distances
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
     * @brief Calculate greedy route
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