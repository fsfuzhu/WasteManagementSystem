// WasteLocation.cpp
// Implementation of the WasteLocation class
#include "WasteLocation.h"
#include <random>
#include <chrono>
#include <iostream>

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

    return locations;
}