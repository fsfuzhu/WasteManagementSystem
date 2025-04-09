// MapVisualization.h
// This file defines the map visualization UI component
#pragma once

#include "../Core/WasteLocation.h"
#include "../Core/Route.h"
#include <vector>

/**
 * @brief Component for visualizing the map and routes
 */
class MapVisualization {
public:
    MapVisualization() = default;
    ~MapVisualization() = default;

    /**
     * @brief Initialize the map visualization
     * @return True if initialization was successful
     */
    bool Initialize() { return true; }

    /**
     * @brief Render the map with current route
     * @param locations Vector of waste locations
     * @param route The current route to display
     */
    void Render(const std::vector<WasteLocation>& locations, const Route* route);
};