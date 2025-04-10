// UIManager.h
// This file manages the ImGui user interface for the waste management system
#pragma once

#include <memory>
#include <string>
#include <vector>

// Forward declarations
struct GLFWwindow;
class Application;

// Include the full class definitions for these components
// This fixes the incomplete type error with std::unique_ptr
#include "MainWindow.h"
#include "MapVisualization.h"
#include "RouteComparisonPanel.h"

/**
 * @brief Manages the ImGui-based user interface
 */
class UIManager {
private:
    /* Private members in UIManager class */
    Application* m_application;                       // Pointer to application
    GLFWwindow* m_window;                            // GLFW window handle

    // UI components
    std::unique_ptr<MainWindow> m_mainWindow;
    std::unique_ptr<MapVisualization> m_mapVisualization;
    std::unique_ptr<RouteComparisonPanel> m_routeComparisonPanel;

    // UI state
    bool m_showMapWindow;
    bool m_showDetailsWindow;
    bool m_showComparisonWindow;
    bool m_showAIWindow;
    bool m_showSettingsWindow;

    // Window size and position tracking
    int m_windowWidth;
    int m_windowHeight;

    // ImGui setup
    void SetupImGuiStyle();
    void SetupImGuiConfig();
    void SetupDockspace();

    // UI windows
    void RenderMenuBar();
    void RenderMapWindow();
    void RenderDetailsWindow();
    void RenderComparisonWindow();
    void RenderAIWindow();
    void RenderSettingsWindow();
    void RenderStatusBar();

    // Helper functions for AI UI
    void RenderWastePredictionUI();
    void RenderRouteLearningUI();
    void RenderLocationClusteringUI();

public:
    UIManager(Application* application);
    ~UIManager();

    /**
     * @brief Initialize the UI manager
     * @param window GLFW window handle
     * @return True if initialization successful
     */
    bool Initialize(GLFWwindow* window);

    /**
     * @brief Begin a new ImGui frame
     */
    void BeginFrame();

    /**
     * @brief Render the UI
     */
    void Render();

    /**
     * @brief End the ImGui frame
     */
    void EndFrame();

    /**
     * @brief Check if the window should close
     * @return True if the window should close
     */
    bool ShouldClose() const;

    /**
     * @brief Handle window resize event
     * @param width New window width
     * @param height New window height
     */
    void HandleResize(int width, int height);
};