// Application.h
// Main application class that manages the waste management system
#pragma once

#include "Core/WasteLocation.h"
#include "Core/Route.h"
#include "Core/NonOptimizedRoute.h"
#include "Core/OptimizedRoute.h"
#include "Core/MSTRoute.h"
#include "Core/TSPRoute.h"
#include "Core/GreedyRoute.h"
#include "AI/WasteLevelPredictor.h"
#include "AI/RouteLearningAgent.h"
#include "AI/LocationClustering.h"

#include <memory>
#include <vector>
#include <string>

// Forward declarations
class GLFWwindow;
class UIManager;

/**
 * @brief Main application class for the Waste Management System
 */
class Application {
private:
    /* Private members in Application class */
    GLFWwindow* m_window;                          // GLFW window handle
    std::unique_ptr<UIManager> m_uiManager;        // UI manager
    std::vector<WasteLocation> m_wasteLocations;   // All waste locations in city

    // Route algorithms
    std::unique_ptr<NonOptimizedRoute> m_nonOptimizedRoute;
    std::unique_ptr<OptimizedRoute> m_optimizedRoute;
    std::unique_ptr<MSTRoute> m_mstRoute;
    std::unique_ptr<TSPRoute> m_tspRoute;
    std::unique_ptr<GreedyRoute> m_greedyRoute;

    // Currently selected route
    Route* m_currentRoute;
    int m_currentRouteIndex;

    // AI components
    std::unique_ptr<WasteLevelPredictor> m_wasteLevelPredictor;
    std::unique_ptr<RouteLearningAgent> m_routeLearningAgent;
    std::unique_ptr<LocationClustering> m_locationClustering;

    // Application state
    bool m_running;
    bool m_showDemoWindow;

    // Helper methods
    void InitializeRouteAlgorithms();
    void InitializeAIComponents();
    void UpdateAIComponents();
    void WriteRouteReport(std::ofstream& file, Route* route, const std::string& title);

public:
    Application();
    ~Application();

    /**
     * @brief Initialize the application
     * @param window GLFW window handle
     * @return True if initialization was successful
     */
    bool Initialize(GLFWwindow* window);

    /**
     * @brief Run the main application loop
     */
    void Run();

    /**
     * @brief Clean up resources
     */
    void Shutdown();

    // Getters
    UIManager* GetUIManager() const;
    const std::vector<WasteLocation>& GetWasteLocations() const;
    Route* GetCurrentRoute() const;
    int GetCurrentRouteIndex() const;
    WasteLevelPredictor* GetWasteLevelPredictor() const;
    RouteLearningAgent* GetRouteLearningAgent() const;
    LocationClustering* GetLocationClustering() const;

    // Route management
    void SelectRoute(int index);
    void RecalculateCurrentRoute();
    void RegenerateWasteLevels();
    void ExportRouteReport(const std::string& filename);

    // AI methods
    void PredictFutureWasteLevels(int daysAhead);
    void OptimizeWithAI();
};