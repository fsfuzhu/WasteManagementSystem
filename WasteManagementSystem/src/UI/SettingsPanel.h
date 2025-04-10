// SettingsPanel.h
// This file defines the settings panel UI component
#pragma once

#include <functional>

/**
 * @brief Panel for application settings
 */
class SettingsPanel {
public:
    /**
     * @brief Constructor for SettingsPanel
     * @param themeToggleCallback Callback for toggling theme
     */
    SettingsPanel(std::function<void()> themeToggleCallback = nullptr);
    ~SettingsPanel() = default;

    /**
     * @brief Initialize the settings panel
     * @return True if initialization was successful
     */
    bool Initialize();

    /**
     * @brief Render the settings panel
     * @param darkTheme Current theme state (true = dark)
     */
    void Render(bool& darkTheme);

    /**
     * @brief Get fuel cost per km
     * @return Fuel cost per km (RM)
     */
    float GetFuelCostPerKm() const { return m_fuelCostPerKm; }

    /**
     * @brief Get driver wage per hour
     * @return Driver wage per hour (RM)
     */
    float GetDriverWagePerHour() const { return m_driverWagePerHour; }

    /**
     * @brief Get driving speed in minutes per km
     * @return Driving speed (min/km)
     */
    float GetDrivingSpeedMinPerKm() const { return m_drivingSpeedMinPerKm; }

private:
    // Callback for theme toggle
    std::function<void()> m_themeToggleCallback;

    // Simulation settings
    float m_fuelCostPerKm = 2.50f;        // Fuel cost in RM per km
    float m_driverWagePerHour = 10.00f;   // Driver wage in RM per hour
    float m_drivingSpeedMinPerKm = 1.5f;  // Driving speed in minutes per km

    // UI settings
    float m_fontScale = 1.0f;             // Font scale factor
};