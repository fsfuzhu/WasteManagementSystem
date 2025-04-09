// RouteComparisonPanel.cpp
// Implementation of the route comparison panel UI component
#include "RouteComparisonPanel.h"
#include "imgui.h"
#include "implot.h"
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>

void RouteComparisonPanel::Render(const std::vector<Route*>& routes) {
    if (routes.empty())
        return;

    // Prepare data for comparison
    std::vector<float> distances;
    std::vector<float> costs;
    std::vector<std::string> labelStrings;
    std::vector<const char*> labels;

    for (const auto& route : routes) {
        if (route) {
            distances.push_back(route->GetTotalDistance());
            costs.push_back(route->GetTotalCost());
            labelStrings.push_back(route->GetRouteName());
            labels.push_back(labelStrings.back().c_str());
        }
    }

    // Render bar chart comparing distances
    if (ImPlot::BeginPlot("Route Distance Comparison", ImVec2(-1, 200))) {
        ImPlot::SetupAxes("Route Type", "Distance (km)");

        // Fix the SetupAxisTicks call to match the correct signature
        // Create array of const char* const for labels
        const char* const* labelArrayDist = labels.data();
        ImPlot::SetupAxisTicks(ImAxis_X1, 0, static_cast<double>(distances.size() - 1),
            static_cast<int>(labels.size()), labelArrayDist, false);

        ImPlot::PlotBars("Distances", distances.data(), static_cast<int>(distances.size()), 0.7f);
        ImPlot::EndPlot();
    }

    // Render bar chart comparing costs
    if (ImPlot::BeginPlot("Route Cost Comparison", ImVec2(-1, 200))) {
        ImPlot::SetupAxes("Route Type", "Cost (RM)");

        // Fix the SetupAxisTicks call to match the correct signature
        const char* const* labelArrayCost = labels.data();
        ImPlot::SetupAxisTicks(ImAxis_X1, 0, static_cast<double>(costs.size() - 1),
            static_cast<int>(labels.size()), labelArrayCost, false);

        ImPlot::PlotBars("Costs", costs.data(), static_cast<int>(costs.size()), 0.7f);
        ImPlot::EndPlot();
    }

    // Comparison table
    ImGui::Separator();
    ImGui::Text("Route Comparison Table:");

    if (ImGui::BeginTable("##RouteComparisonTable", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
        // Headers
        ImGui::TableSetupColumn("Route Type");
        ImGui::TableSetupColumn("Total Distance (km)");
        ImGui::TableSetupColumn("Total Cost (RM)");
        ImGui::TableSetupColumn("Savings vs Non-Optimized (%)");
        ImGui::TableHeadersRow();

        float baseline_cost = 0;
        // Find non-optimized route as baseline
        for (const auto& route : routes) {
            if (route && route->GetRouteName() == "Non-Optimized Route") {
                baseline_cost = route->GetTotalCost();
                break;
            }
        }

        // Data rows
        for (const auto& route : routes) {
            if (!route)
                continue;

            ImGui::TableNextRow();

            // Route name
            ImGui::TableNextColumn();
            ImGui::Text("%s", route->GetRouteName().c_str());

            // Distance
            ImGui::TableNextColumn();
            ImGui::Text("%.2f", route->GetTotalDistance());

            // Cost
            ImGui::TableNextColumn();
            ImGui::Text("%.2f", route->GetTotalCost());

            // Savings
            ImGui::TableNextColumn();
            if (baseline_cost > 0 && route->GetRouteName() != "Non-Optimized Route") {
                float savings = ((baseline_cost - route->GetTotalCost()) / baseline_cost) * 100.0f;
                ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "%.2f%%", savings);
            }
            else {
                ImGui::Text("-");
            }
        }

        ImGui::EndTable();
    }
}