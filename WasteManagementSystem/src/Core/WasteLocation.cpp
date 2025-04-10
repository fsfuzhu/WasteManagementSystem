// WasteLocation.cpp
// Implementation of the WasteLocation class
#include "pch.h"
#include "WasteLocation.h"

// Initialize static distance matrix
float WasteLocation::map_distance_matrix[8][8];

// Constructor
WasteLocation::WasteLocation(const std::string& name)
    : m_locationName(name)
{
    // Generate random waste level on creation
    RegenerateWasteLevel();
}

// Generate random waste level
void WasteLocation::RegenerateWasteLevel()
{
    // Set up random number generator
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> dist(0.0f, 100.0f);

    // Generate random waste level between 0% and 100%
    const_cast<float&>(m_wasteLevel) = std::round(dist(gen));
}

// Get waste level
float WasteLocation::GetWasteLevel() const
{
    return m_wasteLevel;
}

// Get location name
std::string WasteLocation::GetLocationName() const
{
    return m_locationName;
}

// Set waste level (for testing or specific scenarios)
void WasteLocation::SetWasteLevel(float level)
{
    // Ensure level is between 0 and 100
    level = std::max(0.0f, std::min(100.0f, level));

    // Set waste level
    const_cast<float&>(m_wasteLevel) = level;
}

// Set location name
void WasteLocation::SetLocationName(const std::string& name)
{
    m_locationName = name;
}

// Initialize vector of waste locations
std::vector<WasteLocation> WasteLocation::InitializeWasteLocations()
{
    std::vector<WasteLocation> locations;

    // Create waste locations
    locations.push_back(WasteLocation("A"));
    locations.push_back(WasteLocation("B"));
    locations.push_back(WasteLocation("C"));
    locations.push_back(WasteLocation("D"));
    locations.push_back(WasteLocation("E"));
    locations.push_back(WasteLocation("F"));
    locations.push_back(WasteLocation("G"));

    // Initialize distance matrix
    InitializeDistanceMatrix();

    return locations;
}

// Calculate direct distance between two locations based on coordinates
float WasteLocation::CalculateDirectDistance(int fromId, int toId)
{
    if (fromId == toId) {
        return 0.0f; // Distance to self is 0
    }

    // Get coordinates
    float x1 = location_coordinates[fromId][0];
    float y1 = location_coordinates[fromId][1];
    float x2 = location_coordinates[toId][0];
    float y2 = location_coordinates[toId][1];

    // Calculate euclidean distance
    float dx = x2 - x1;
    float dy = y2 - y1;
    float distance = std::sqrt(dx * dx + dy * dy) / 50.0f; // Scale factor to get reasonable distances in km

    // Round to 1 decimal place
    return std::round(distance * 10.0f) / 10.0f;
}

// Initialize distance matrix with direct distances
void WasteLocation::InitializeDistanceMatrix()
{
    // Calculate distances between all pairs of locations
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            map_distance_matrix[i][j] = CalculateDirectDistance(i, j);
        }
    }
}