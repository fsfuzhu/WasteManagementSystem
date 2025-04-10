// RouteDetailsPanel.h
// This file defines the route details panel UI component
#pragma once

#include "../Core/Route.h"
#include "../Core/WasteLocation.h"
#include <vector>

/**
 * @brief Panel for displaying route details
 */
class RouteDetailsPanel {
public:
    RouteDetailsPanel() = default;
    ~RouteDetailsPanel() = default;

    /**
     * @brief Initialize the details panel
     * @return True if initialization was successful
     */
    bool Initialize();

    /**
     * @brief Render the route details panel
     * @param locations Vector of waste locations
     * @param route Current active route
     */
    void Render(const std::vector<WasteLocation>& locations, const Route* route);

private:
    /**
     * @brief Display waste levels section
     * @param locations Vector of waste locations
     */
    void RenderWasteLevels(const std::vector<WasteLocation>& locations);

    /**
     * @brief Display route sequence section
     * @param route Current active route
     */
    void RenderRouteSequence(const Route* route);

    /**
     * @brief Display route segment details
     * @param route Current active route
     */
    void RenderSegmentDetails(const Route* route);

    /**
     * @brief Display cost summary
     * @param route Current active route
     */
    void RenderCostSummary(const Route* route);
};