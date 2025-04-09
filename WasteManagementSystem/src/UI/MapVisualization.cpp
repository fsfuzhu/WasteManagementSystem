// MapVisualization.cpp
// Implementation of the map visualization UI component
#include "MapVisualization.h"
#include "imgui.h"
#include <iostream>

void MapVisualization::Render(const std::vector<WasteLocation>& locations, const Route* route) {
    if (locations.empty())
        return;

    // Window size
    ImVec2 windowSize = ImGui::GetContentRegionAvail();

    // Draw map background
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 canvasPos = ImGui::GetCursorScreenPos();
    ImVec2 canvasSize = windowSize;

    // Background
    drawList->AddRectFilled(canvasPos,
        ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y),
        IM_COL32(30, 30, 40, 255));

    // Map grid
    float gridSize = 50.0f;
    ImU32 gridColor = IM_COL32(50, 50, 60, 100);

    for (float x = 0.0f; x < canvasSize.x; x += gridSize) {
        drawList->AddLine(ImVec2(canvasPos.x + x, canvasPos.y),
            ImVec2(canvasPos.x + x, canvasPos.y + canvasSize.y),
            gridColor);
    }

    for (float y = 0.0f; y < canvasSize.y; y += gridSize) {
        drawList->AddLine(ImVec2(canvasPos.x, canvasPos.y + y),
            ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + y),
            gridColor);
    }

    // Center and scale the map
    float scaleX = canvasSize.x / 500.0f;
    float scaleY = canvasSize.y / 500.0f;
    float scale = std::min(scaleX, scaleY);

    float offsetX = canvasPos.x + (canvasSize.x - 500.0f * scale) * 0.5f;
    float offsetY = canvasPos.y + (canvasSize.y - 500.0f * scale) * 0.5f;

    // Draw roads
    ImU32 roadColor = IM_COL32(150, 150, 150, 200);
    float roadThickness = 3.0f;

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (WasteLocation::map_distance_matrix[i][j] < INF) {
                // Draw road from location i to j
                ImVec2 p1(
                    offsetX + WasteLocation::location_coordinates[i][0] * scale,
                    offsetY + WasteLocation::location_coordinates[i][1] * scale
                );

                ImVec2 p2(
                    offsetX + WasteLocation::location_coordinates[j][0] * scale,
                    offsetY + WasteLocation::location_coordinates[j][1] * scale
                );

                drawList->AddLine(p1, p2, roadColor, roadThickness);

                // Draw distance label
                ImVec2 midpoint(
                    (p1.x + p2.x) * 0.5f,
                    (p1.y + p2.y) * 0.5f
                );

                // Convert distance to string
                char distStr[16];
                snprintf(distStr, sizeof(distStr), "%.1f", WasteLocation::map_distance_matrix[i][j]);

                drawList->AddText(midpoint, IM_COL32(255, 255, 255, 200), distStr);
            }
        }
    }

    // Draw route if available
    if (route) {
        const std::vector<int>& routePath = route->GetFinalRoute();

        if (routePath.size() > 1) {
            // Route color
            ImU32 routeColor = IM_COL32(0, 255, 0, 200);
            float routeThickness = 4.0f;

            // Draw route segments
            for (size_t i = 0; i < routePath.size() - 1; i++) {
                int from = routePath[i];
                int to = routePath[i + 1];

                ImVec2 p1(
                    offsetX + WasteLocation::location_coordinates[from][0] * scale,
                    offsetY + WasteLocation::location_coordinates[from][1] * scale
                );

                ImVec2 p2(
                    offsetX + WasteLocation::location_coordinates[to][0] * scale,
                    offsetY + WasteLocation::location_coordinates[to][1] * scale
                );

                // Draw direction arrow
                drawList->AddLine(p1, p2, routeColor, routeThickness);

                // Draw arrow
                float angle = atan2(p2.y - p1.y, p2.x - p1.x);
                float arrowSize = 10.0f;

                ImVec2 arrowPos(
                    p1.x + (p2.x - p1.x) * 0.6f,
                    p1.y + (p2.y - p1.y) * 0.6f
                );

                drawList->AddTriangleFilled(
                    arrowPos,
                    ImVec2(arrowPos.x - arrowSize * cosf(angle + 0.5f),
                        arrowPos.y - arrowSize * sinf(angle + 0.5f)),
                    ImVec2(arrowPos.x - arrowSize * cosf(angle - 0.5f),
                        arrowPos.y - arrowSize * sinf(angle - 0.5f)),
                    routeColor
                );

                // Add order number
                char orderStr[8];
                snprintf(orderStr, sizeof(orderStr), "%zu", i + 1);

                ImVec2 orderPos(
                    p1.x + (p2.x - p1.x) * 0.5f - 8.0f,
                    p1.y + (p2.y - p1.y) * 0.5f - 8.0f
                );

                drawList->AddCircleFilled(orderPos, 12.0f, IM_COL32(0, 0, 0, 200));
                drawList->AddText(
                    ImVec2(orderPos.x - 4.0f, orderPos.y - 7.0f),
                    IM_COL32(255, 255, 255, 255),
                    orderStr
                );
            }
        }
    }

    // Draw waste locations
    for (size_t i = 0; i < 8; i++) {
        ImVec2 pos(
            offsetX + WasteLocation::location_coordinates[i][0] * scale,
            offsetY + WasteLocation::location_coordinates[i][1] * scale
        );

        // Determine color based on location type
        ImU32 color;
        float radius;

        if (i == 0) {
            // Station
            color = IM_COL32(255, 165, 0, 255); // Orange
            radius = 15.0f;
        }
        else {
            // Waste location
            float wasteLevel = 0.0f;

            // Find waste level for this location
            for (const auto& loc : locations) {
                if (loc.GetLocationName() == WasteLocation::dict_Id_to_Name[i]) {
                    wasteLevel = loc.GetWasteLevel();
                    break;
                }
            }

            // Color based on waste level
            if (wasteLevel < 30.0f) {
                color = IM_COL32(0, 255, 0, 255); // Green
            }
            else if (wasteLevel < 60.0f) {
                color = IM_COL32(255, 255, 0, 255); // Yellow
            }
            else {
                color = IM_COL32(255, 0, 0, 255); // Red
            }

            radius = 10.0f;
        }

        // Draw location
        drawList->AddCircleFilled(pos, radius, color);
        drawList->AddCircle(pos, radius, IM_COL32(255, 255, 255, 200), 0, 2.0f);

        // Draw location label
        ImGui::SetCursorScreenPos(ImVec2(pos.x - 15.0f, pos.y - 35.0f));

        // Get location name
        std::string name = WasteLocation::dict_Id_to_Name[i];

        // Add waste level if not station
        if (i > 0) {
            float wasteLevel = 0.0f;

            // Find waste level for this location
            for (const auto& loc : locations) {
                if (loc.GetLocationName() == name) {
                    wasteLevel = loc.GetWasteLevel();
                    break;
                }
            }

            name += " (" + std::to_string(static_cast<int>(wasteLevel)) + "%)";
        }

        ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "%s", name.c_str());
    }

    // Reset cursor position
    ImGui::SetCursorScreenPos(canvasPos);
    ImGui::Dummy(canvasSize);
}