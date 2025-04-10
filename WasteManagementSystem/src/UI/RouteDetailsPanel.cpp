// RouteDetailsPanel.cpp
// Implementation of the route details panel
#include "pch.h"
#include "UI/RouteDetailsPanel.h"
#include "UI/UIHelpers.h"

bool RouteDetailsPanel::Initialize()
{
    return true;
}

void RouteDetailsPanel::Render(const std::vector<WasteLocation>& locations, const Route* route)
{
    if (!route) {
        ImGui::Text("No route selected.");
        return;
    }

    // Route name and waste threshold
    ImGui::Text("Route Type: %s", route->GetRouteName().c_str());
    ImGui::Text("Waste Threshold: %.0f%%", route->GetWasteThreshold());
    ImGui::Separator();

    // Display waste levels
    RenderWasteLevels(locations);

    // Display route information
    const std::vector<int>& routePath = route->GetFinalRoute();

    if (routePath.empty()) {
        ImGui::Text("No route calculated.");
        return;
    }

    // Route sequence
    RenderRouteSequence(route);

    // Route segments
    RenderSegmentDetails(route);

    // Route costs
    RenderCostSummary(route);
}

void RouteDetailsPanel::RenderWasteLevels(const std::vector<WasteLocation>& locations)
{
    UIHelpers::DrawSectionHeader("Waste Levels:");

    ImGui::Columns(2, "waste_levels");
    ImGui::Text("Location"); ImGui::NextColumn();
    ImGui::Text("Waste Level (%)"); ImGui::NextColumn();
    ImGui::Separator();

    for (const auto& location : locations) {
        ImGui::Text("%s", location.GetLocationName().c_str());
        ImGui::NextColumn();

        // Color based on waste level
        float wasteLevel = location.GetWasteLevel();
        ImVec4 color;

        if (wasteLevel < 30.0f) {
            color = ImVec4(0.0f, 1.0f, 0.0f, 1.0f); // Green
        }
        else if (wasteLevel < 60.0f) {
            color = ImVec4(1.0f, 1.0f, 0.0f, 1.0f); // Yellow
        }
        else {
            color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f); // Red
        }

        UIHelpers::DrawColoredText(std::to_string(static_cast<int>(wasteLevel)).c_str(), color);
        ImGui::NextColumn();
    }

    ImGui::Columns(1);
    ImGui::Separator();
}

void RouteDetailsPanel::RenderRouteSequence(const Route* route)
{
    UIHelpers::DrawSectionHeader("Route Sequence:");

    const std::vector<int>& routePath = route->GetFinalRoute();

    // Display route as Station -> A -> B -> Station
    std::string routeStr;
    for (size_t i = 0; i < routePath.size(); i++) {
        routeStr += WasteLocation::dict_Id_to_Name[routePath[i]];
        if (i < routePath.size() - 1) {
            routeStr += " -> ";
        }
    }

    ImGui::TextWrapped("%s", routeStr.c_str());
    ImGui::Separator();
}

void RouteDetailsPanel::RenderSegmentDetails(const Route* route)
{
    UIHelpers::DrawSectionHeader("Route Segments:");

    const std::vector<int>& routePath = route->GetFinalRoute();
    const std::vector<float>& distances = route->GetIndividualDistances();

    ImGui::Columns(2, "route_segments");
    ImGui::Text("Segment"); ImGui::NextColumn();
    ImGui::Text("Distance (km)"); ImGui::NextColumn();
    ImGui::Separator();

    for (size_t i = 0; i < routePath.size() - 1; i++) {
        std::string segment = WasteLocation::dict_Id_to_Name[routePath[i]] + " -> " +
            WasteLocation::dict_Id_to_Name[routePath[i + 1]];
        ImGui::Text("%s", segment.c_str());
        ImGui::NextColumn();

        ImGui::Text("%.2f", distances[i]);
        ImGui::NextColumn();
    }

    ImGui::Columns(1);
    ImGui::Separator();
}

void RouteDetailsPanel::RenderCostSummary(const Route* route)
{
    UIHelpers::DrawSectionHeader("Route Costs:");

    ImGui::Columns(2, "route_costs");

    UIHelpers::DrawLabeledValue("Total Distance", route->GetTotalDistance(), "%.2f km");

    ImGui::Text("Time Taken:"); ImGui::NextColumn();
    ImGui::Text("%.2f min (%.2f hours)",
        route->GetTimeTaken(),
        route->GetTimeTaken() / 60.0f);
    ImGui::NextColumn();

    UIHelpers::DrawLabeledValue("Fuel Consumption", route->GetFuelConsumption(), "RM %.2f");
    UIHelpers::DrawLabeledValue("Driver's Wage", route->GetWage(), "RM %.2f");

    ImGui::Text("Total Cost:"); ImGui::NextColumn();
    ImGui::TextColored(ImVec4(1.0f, 0.84f, 0.0f, 1.0f), "RM %.2f", route->GetTotalCost());
    ImGui::NextColumn();

    ImGui::Columns(1);
}