// Route.h
// This file defines the base Route class that all route types will inherit from
#pragma once

#include "WasteLocation.h"
#include <vector>
#include <string>

/**
 * @brief Base class for all route types
 */
class Route {
protected:
    /* Protected members in Route class */
    std::vector<int> m_finalRoute;          // Sequence of location IDs to visit
    std::vector<float> m_individualDistances; // Distance between consecutive locations
    float m_totalDistance;                  // Total route distance in km
    float m_timeTaken;                      // Total time in minutes
    float m_fuelConsumption;                // Fuel cost in RM
    float m_wage;                           // Driver's wage in RM
    float m_totalCost;                      // Total cost in RM
    std::string m_routeName;                // Name of the route type
    float m_wasteThreshold;                 // Minimum waste level threshold for collection

    // Calculate costs based on distances
    void CalculateCosts();

public:
    /**
     * @brief Constructor for Route
     * @param name Name of the route type
     * @param threshold Waste threshold level (%)
     */
    Route(const std::string& name, float threshold);
    virtual ~Route();

    /**
     * @brief Calculate the optimized route
     * @param locations Vector of waste locations to consider
     * @return True if a valid route was found, false otherwise
     */
    virtual bool CalculateRoute(const std::vector<WasteLocation>& locations) = 0;

    // Getters
    const std::vector<int>& GetFinalRoute() const;
    const std::vector<float>& GetIndividualDistances() const;
    float GetTotalDistance() const;
    float GetTimeTaken() const;
    float GetFuelConsumption() const;
    float GetWage() const;
    float GetTotalCost() const;
    std::string GetRouteName() const;
    float GetWasteThreshold() const;
};