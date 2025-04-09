// RouteComparisonPanel.h
// This file defines the route comparison panel UI component
#pragma once

#include "../Core/Route.h"
#include <vector>

/**
 * @brief Panel for comparing different route algorithms
 */
class RouteComparisonPanel {
public:
    RouteComparisonPanel() = default;
    ~RouteComparisonPanel() = default;

    /**
     * @brief Initialize the comparison panel
     * @return True if initialization was successful
     */
    bool Initialize() { return true; }

    /**
     * @brief Render the comparison panel
     * @param routes Vector of routes to compare
     */
    void Render(const std::vector<Route*>& routes) {}
};