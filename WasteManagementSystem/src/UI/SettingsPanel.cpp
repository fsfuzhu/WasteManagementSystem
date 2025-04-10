// SettingsPanel.cpp
// Implementation of the settings panel UI component
#include "pch.h"
#include "UI/SettingsPanel.h"
#include "UI/UIHelpers.h"

SettingsPanel::SettingsPanel(std::function<void()> themeToggleCallback)
    : m_themeToggleCallback(themeToggleCallback),
    m_fuelCostPerKm(2.50f),
    m_driverWagePerHour(10.00f),
    m_drivingSpeedMinPerKm(1.5f),
    m_fontScale(1.0f)
{
}

bool SettingsPanel::Initialize()
{
    return true;
}

void SettingsPanel::Render(bool& darkTheme)
{
    // Appearance settings
    UIHelpers::DrawSectionHeader("Appearance Settings");

    // Theme toggle
    bool isDarkTheme = darkTheme;
    if (ImGui::Checkbox("Dark Theme", &isDarkTheme)) {
        darkTheme = isDarkTheme;
        if (m_themeToggleCallback) {
            m_themeToggleCallback();
        }
    }

    // Font scale slider
    ImGui::SliderFloat("Font Size", &m_fontScale, 0.7f, 1.5f, "%.1f");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Changes will take effect after restart");
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Simulation settings
    UIHelpers::DrawSectionHeader("Simulation Settings");

    // Cost settings
    ImGui::Text("Cost Parameters:");

    // Fuel cost per km
    ImGui::PushItemWidth(150);
    if (ImGui::InputFloat("Fuel Cost per km (RM)", &m_fuelCostPerKm, 0.10f, 0.50f, "%.2f")) {
        // Ensure value is positive
        m_fuelCostPerKm = std::max(0.1f, m_fuelCostPerKm);
    }
    UIHelpers::AddTooltip("Cost of fuel per kilometer driven");

    // Driver wage per hour
    if (ImGui::InputFloat("Driver Wage per hour (RM)", &m_driverWagePerHour, 1.00f, 5.00f, "%.2f")) {
        // Ensure value is positive
        m_driverWagePerHour = std::max(1.0f, m_driverWagePerHour);
    }
    UIHelpers::AddTooltip("Hourly wage paid to the driver");

    // Driving speed
    if (ImGui::InputFloat("Driving Time per km (min)", &m_drivingSpeedMinPerKm, 0.10f, 0.50f, "%.2f")) {
        // Ensure value is positive
        m_drivingSpeedMinPerKm = std::max(0.5f, m_drivingSpeedMinPerKm);
    }
    UIHelpers::AddTooltip("Average time taken to drive one kilometer");

    ImGui::PopItemWidth();

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Route algorithm settings
    UIHelpers::DrawSectionHeader("Route Algorithm Settings");

    // These settings would actually be stored in the respective route classes
    // But we include them here for completeness

    ImGui::Text("Waste Level Thresholds:");

    // Route thresholds - these are just UI elements, not connected to actual route objects yet
    static float nonOptimizedThreshold = 40.0f;
    static float optimizedThreshold = 60.0f;
    static float mstThreshold = 40.0f;
    static float tspThreshold = 40.0f;
    static float greedyThreshold = 30.0f;

    ImGui::PushItemWidth(200);

    ImGui::SliderFloat("Non-Optimized Route", &nonOptimizedThreshold, 0.0f, 100.0f, "%.0f%%");
    UIHelpers::AddTooltip("Waste locations with level >= threshold will be visited");

    ImGui::SliderFloat("Optimized Route", &optimizedThreshold, 0.0f, 100.0f, "%.0f%%");
    UIHelpers::AddTooltip("Waste locations with level >= threshold will be visited");

    ImGui::SliderFloat("MST Route", &mstThreshold, 0.0f, 100.0f, "%.0f%%");
    UIHelpers::AddTooltip("Waste locations with level >= threshold will be visited");

    ImGui::SliderFloat("TSP Route", &tspThreshold, 0.0f, 100.0f, "%.0f%%");
    UIHelpers::AddTooltip("Waste locations with level >= threshold will be visited");

    ImGui::SliderFloat("Greedy Route", &greedyThreshold, 0.0f, 100.0f, "%.0f%%");
    UIHelpers::AddTooltip("Waste locations with level >= threshold will be visited");

    ImGui::PopItemWidth();

    ImGui::Spacing();
    ImGui::Text("Distance Limits:");

    // Distance limits - these are just UI elements, not connected to actual route objects yet
    static float nonOptimizedLimit = 30.0f;
    static float optimizedLimit = 20.0f;
    static float mstLimit = 15.0f;
    static float tspLimit = 15.0f;
    static bool greedyNoLimit = true;

    ImGui::PushItemWidth(200);

    ImGui::SliderFloat("Non-Optimized Limit", &nonOptimizedLimit, 5.0f, 50.0f, "%.0f km");
    ImGui::SliderFloat("Optimized Limit", &optimizedLimit, 5.0f, 50.0f, "%.0f km");
    ImGui::SliderFloat("MST Limit", &mstLimit, 5.0f, 50.0f, "%.0f km");
    ImGui::SliderFloat("TSP Limit", &tspLimit, 5.0f, 50.0f, "%.0f km");

    ImGui::Checkbox("Greedy Route - No Distance Limit", &greedyNoLimit);
    if (!greedyNoLimit) {
        static float greedyLimit = 50.0f;
        ImGui::SliderFloat("Greedy Limit", &greedyLimit, 5.0f, 100.0f, "%.0f km");
    }

    ImGui::PopItemWidth();

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Default settings reset
    if (ImGui::Button("Reset to Default Settings", ImVec2(200, 30))) {
        // Confirm dialog using UIHelpers
        UIHelpers::DrawConfirmButton("Reset to Default Settings",
            "Are you sure you want to reset all settings to their default values?",
            [this, &darkTheme]() {
                // Reset all settings to default
                m_fuelCostPerKm = 2.50f;
                m_driverWagePerHour = 10.00f;
                m_drivingSpeedMinPerKm = 1.5f;
                m_fontScale = 1.0f;
                darkTheme = true;

                // Reset route settings
                nonOptimizedThreshold = 40.0f;
                optimizedThreshold = 60.0f;
                mstThreshold = 40.0f;
                tspThreshold = 40.0f;
                greedyThreshold = 30.0f;

                nonOptimizedLimit = 30.0f;
                optimizedLimit = 20.0f;
                mstLimit = 15.0f;
                tspLimit = 15.0f;
                greedyNoLimit = true;

                // Call theme toggle callback to update theme
                if (m_themeToggleCallback) {
                    m_themeToggleCallback();
                }
            }
        );
    }

    // Note about settings persistence
    ImGui::Spacing();
    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f),
        "Note: Settings are not yet persisted between application sessions.");
}