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

    /* Static public variables shared across all instances of WasteLocation class */

    // Distance matrix between locations [from][to] = distance
    // Size is 8x8 for 7 waste locations + 1 station
    // INF means no direct path between locations
    static inline const float map_distance_matrix[8][8] = {
        {0, 3, INF, INF, INF, INF, INF, 4},
        {3, 0, INF, 6, INF, INF, INF, INF},
        {INF, INF, 0, 5, 4, INF, INF, INF},
        {INF, 6, 5, 0, 2, INF, 2, INF},
        {INF, INF, 4, 2, 0, INF, INF, INF},
        {INF, INF, INF, INF, INF, 0, 7, INF},
        {INF, INF, INF, 2, INF, 7, 0, 3},
        {4, INF, INF, INF, INF, INF, 3, 0}
    };

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
};