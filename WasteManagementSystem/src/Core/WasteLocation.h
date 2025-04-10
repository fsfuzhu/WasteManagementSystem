// WasteLocation.h
// This file defines a WasteLocation Class which represents each waste collection pick-up
// point within the city
#pragma once

#include <string>
#include <vector>
#include <map>
#include <limits>
#include <cmath>
#include <random>

// Define INF constant for unreachable locations
#define INF (std::numeric_limits<float>::max())

/**
 * @brief This class represents the waste pick-up locations in the city
 */
class WasteLocation {
private:
    /* Private members in WasteLocation class */
    float m_wasteLevel;            // Waste level assigned randomly (0-100%)
    std::string m_locationName;    // Name of the waste location

public:
    /**
     * @brief Constructor for WasteLocation
     * @param name Name of the waste location
     */
    WasteLocation(const std::string& name);

    // Generate random waste level for testing or simulation
    void RegenerateWasteLevel();

    // Getters and setters
    float GetWasteLevel() const;
    std::string GetLocationName() const;
    void SetWasteLevel(float level);
    void SetLocationName(const std::string& name);

    /**
     * @brief Initialize a vector of all waste locations in the city
     * @return Vector of WasteLocation objects
     */
    static std::vector<WasteLocation> InitializeWasteLocations();

    /**
     * @brief Calculate direct distance between two locations based on coordinates
     * @param fromId Index of the first location
     * @param toId Index of the second location
     * @return Direct distance in km
     */
    static float CalculateDirectDistance(int fromId, int toId);

    /* Static public variables shared across all instances of WasteLocation class */

    // Dictionary to map location name to its index in the matrix
    static inline std::map<std::string, int> dict_Name_toId = {
        {"Station", 0},
        {"A", 1},
        {"B", 2},
        {"C", 3},
        {"D", 4},
        {"E", 5},
        {"F", 6},
        {"G", 7}
    };

    // Dictionary to map location index to its name
    static inline std::map<int, std::string> dict_Id_to_Name = {
        {0, "Station"},
        {1, "A"},
        {2, "B"},
        {3, "C"},
        {4, "D"},
        {5, "E"},
        {6, "F"},
        {7, "G"}
    };

    // 2D coordinates for drawing the map visualization
    static inline const float location_coordinates[8][2] = {
        {100, 100},  // Station
        {300, 100},  // A
        {400, 300},  // B
        {300, 300},  // C
        {200, 400},  // D
        {100, 400},  // E
        {200, 300},  // F
        {100, 200}   // G
    };

    // Distance matrix between locations will be calculated dynamically based on coordinates
    static float map_distance_matrix[8][8];

    // Initialize distance matrix with direct distances
    static void InitializeDistanceMatrix();
};