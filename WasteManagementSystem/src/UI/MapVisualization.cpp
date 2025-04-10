// MapVisualization.cpp
// Implementation of the map visualization UI component
#include "pch.h"
#include "MapVisualization.h"

void MapVisualization::Render(const std::vector<WasteLocation>& locations, const Route* route)
{
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

    // Draw route if available (only show the green route)
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

                // Add order number with background for better visibility
                char orderStr[8];
                snprintf(orderStr, sizeof(orderStr), "%zu", i + 1);

                ImVec2 orderPos(
                    p1.x + (p2.x - p1.x) * 0.5f,
                    p1.y + (p2.y - p1.y) * 0.5f
                );

                // Add circle background
                drawList->AddCircleFilled(orderPos, 12.0f, IM_COL32(0, 0, 0, 200));

                // Calculate text size and center it properly
                ImVec2 textSize = ImGui::CalcTextSize(orderStr);
                drawList->AddText(
                    ImVec2(orderPos.x - textSize.x * 0.5f, orderPos.y - textSize.y * 0.5f),
                    IM_COL32(255, 255, 255, 255),
                    orderStr
                );

                ImVec2 distPos(
                    p1.x + (p2.x - p1.x) * 0.5f,
                    p1.y + (p2.y - p1.y) * 0.5f - 30.0f  // 改为 -30.0f 增加垂直距离
                );

                // Add distance label
                char distStr[16];
                snprintf(distStr, sizeof(distStr), "%.1f", WasteLocation::map_distance_matrix[from][to]);

                // Background for distance label
                ImVec2 distTextSize = ImGui::CalcTextSize(distStr);
                drawList->AddRectFilled(
                    ImVec2(distPos.x - distTextSize.x * 0.5f - 2, distPos.y - 2),
                    ImVec2(distPos.x + distTextSize.x * 0.5f + 2, distPos.y + distTextSize.y + 2),
                    IM_COL32(0, 0, 0, 180)
                );

                drawList->AddText(
                    ImVec2(distPos.x - distTextSize.x * 0.5f, distPos.y),
                    IM_COL32(255, 255, 255, 255),
                    distStr
                );
            }
        }
    }

    // Draw waste locations and their labels on top
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

        // Draw location label above the marker
        ImVec2 textSize = ImGui::CalcTextSize(name.c_str());
        ImVec2 labelPos(pos.x - textSize.x * 0.5f, pos.y - radius - textSize.y - 5.0f);

        // Add a background for better readability
        drawList->AddRectFilled(
            ImVec2(labelPos.x - 2, labelPos.y - 2),
            ImVec2(labelPos.x + textSize.x + 2, labelPos.y + textSize.y + 2),
            IM_COL32(0, 0, 0, 180)
        );

        drawList->AddText(labelPos, IM_COL32(255, 255, 255, 255), name.c_str());
    }

    // Reset cursor position
    ImGui::SetCursorScreenPos(canvasPos);
    ImGui::Dummy(canvasSize);
}