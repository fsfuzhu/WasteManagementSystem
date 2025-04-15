// Application.cpp
// Implementation of the main application class
#include "pch.h"
#include "Application.h"
#include "UI/UIManager.h"
#include "Utils/FileIO.h"

Application::Application()
    : m_window(nullptr),
    m_uiManager(nullptr),
    m_currentRoute(nullptr),
    m_currentRouteIndex(0),
    m_running(false),
    m_fuelCostPerKm(1.5f),          
    m_driverWagePerHour(6.0f),   
    m_drivingSpeedMinPerKm(1.5f) 
{
}

Application::~Application()
{
    //Shutdown();
}

bool Application::Initialize(GLFWwindow* window)
{
    m_window = window;

    // Initialize UI manager
    m_uiManager = std::make_unique<UIManager>(this);
    if (!m_uiManager->Initialize(window)) {
        std::cerr << "Failed to initialize UI manager" << std::endl;
        return false;
    }

    // Initialize waste locations
    m_wasteLocations = WasteLocation::InitializeWasteLocations();

    // Initialize route algorithms
    InitializeRouteAlgorithms();

    // Initialize AI components
    InitializeAIComponents();

    // Initially select non-optimized route
    SelectRoute(0);

    m_running = true;

    return true;
}

void Application::InitializeRouteAlgorithms()
{
    // Initialize Floyd-Warshall algorithm matrices
    OptimizedRoute::InitializeFloydWarshall();

    // Create route algorithm instances
    m_nonOptimizedRoute = std::make_unique<NonOptimizedRoute>();
    m_optimizedRoute = std::make_unique<OptimizedRoute>();
    m_mstRoute = std::make_unique<MSTRoute>();
    m_tspRoute = std::make_unique<TSPRoute>();
    m_greedyRoute = std::make_unique<GreedyRoute>();

    m_nonOptimizedRoute->SetApplication(this);
    m_optimizedRoute->SetApplication(this);
    m_mstRoute->SetApplication(this);
    m_tspRoute->SetApplication(this);
    m_greedyRoute->SetApplication(this);

    // Calculate initial routes
    m_nonOptimizedRoute->CalculateRoute(m_wasteLocations);
    m_optimizedRoute->CalculateRoute(m_wasteLocations);
    m_mstRoute->CalculateRoute(m_wasteLocations);
    m_tspRoute->CalculateRoute(m_wasteLocations);
    m_greedyRoute->CalculateRoute(m_wasteLocations);
}

void Application::InitializeAIComponents()
{
    // Create AI component instances
    m_wasteLevelPredictor = std::make_unique<WasteLevelPredictor>();
    m_routeLearningAgent = std::make_unique<RouteLearningAgent>();
    m_locationClustering = std::make_unique<LocationClustering>();

    // Initialize AI components
    m_wasteLevelPredictor->Initialize();
    // Additional AI component initialization would go here
}

void Application::Run()
{
    // Main application loop
    while (m_running && !m_uiManager->ShouldClose()) {
        // Poll events
        glfwPollEvents();

        // Update AI components
        UpdateAIComponents();

        // Render UI
        m_uiManager->BeginFrame();
        m_uiManager->Render();
        m_uiManager->EndFrame();

        // Swap buffers
        glfwSwapBuffers(m_window);
    }
}

void Application::UpdateAIComponents()
{
    // Update waste level predictor with current waste levels
    m_wasteLevelPredictor->UpdateModel(m_wasteLocations);

}

void Application::Shutdown()
{
    // Clean up route algorithms
    m_nonOptimizedRoute.reset();
    m_optimizedRoute.reset();
    m_mstRoute.reset();
    m_tspRoute.reset();
    m_greedyRoute.reset();

    // Clean up AI components
    m_wasteLevelPredictor.reset();
    m_routeLearningAgent.reset();
    m_locationClustering.reset();

    m_running = false;
}

void Application::FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);

    // Update UI manager with new size
    Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
    if (app && app->GetUIManager()) {
        app->GetUIManager()->HandleResize(width, height);
    }
}

UIManager* Application::GetUIManager() const
{
    return m_uiManager.get();
}

const std::vector<WasteLocation>& Application::GetWasteLocations() const
{
    return m_wasteLocations;
}

Route* Application::GetCurrentRoute() const
{
    return m_currentRoute;
}

int Application::GetCurrentRouteIndex() const
{
    return m_currentRouteIndex;
}

WasteLevelPredictor* Application::GetWasteLevelPredictor() const
{
    return m_wasteLevelPredictor.get();
}

RouteLearningAgent* Application::GetRouteLearningAgent() const
{
    return m_routeLearningAgent.get();
}

LocationClustering* Application::GetLocationClustering() const
{
    return m_locationClustering.get();
}

void Application::SelectRoute(int index)
{
    m_currentRouteIndex = index;

    // Select the route algorithm
    switch (index) {
    case 0:
        m_currentRoute = m_nonOptimizedRoute.get();
        break;
    case 1:
        m_currentRoute = m_optimizedRoute.get();
        break;
    case 2:
        m_currentRoute = m_mstRoute.get();
        break;
    case 3:
        m_currentRoute = m_tspRoute.get();
        break;
    case 4:
        m_currentRoute = m_greedyRoute.get();
        break;
    default:
        m_currentRoute = m_nonOptimizedRoute.get();
        m_currentRouteIndex = 0;
        break;
    }
}

void Application::RecalculateCurrentRoute()
{
    // Recalculate all routes
    m_nonOptimizedRoute->CalculateRoute(m_wasteLocations);
    m_optimizedRoute->CalculateRoute(m_wasteLocations);
    m_mstRoute->CalculateRoute(m_wasteLocations);
    m_tspRoute->CalculateRoute(m_wasteLocations);
    m_greedyRoute->CalculateRoute(m_wasteLocations);
}

void Application::RegenerateLocations(bool regenerateWasteLevel)
{
    // Regenerate random location coordinates
    WasteLocation::RegenerateLocations();

    // Optionally regenerate waste levels
    if (regenerateWasteLevel) {
        RegenerateWasteLevels();
    }
    else {
        // Just recalculate routes with new distances
        RecalculateCurrentRoute();
    }
}
void Application::RegenerateWasteLevels()
{
    // Regenerate waste levels for all locations
    for (auto& location : m_wasteLocations) {
        location.RegenerateWasteLevel();
    }

    // Recalculate routes with new waste levels
    RecalculateCurrentRoute();
}

void Application::ExportRouteReport(const std::string& filename)
{
    // Define m_routes as a vector of Route pointers to hold all route algorithms  
    std::vector<Route*> m_routes = {
        m_nonOptimizedRoute.get(),
        m_optimizedRoute.get(),
        m_mstRoute.get(),
        m_tspRoute.get(),
        m_greedyRoute.get()
    };

    // Generate predictions if WasteLevelPredictor is available  
    std::unordered_map<std::string, std::vector<float>> predictions;
    if (m_wasteLevelPredictor) {
        predictions = m_wasteLevelPredictor->GenerateForecasts(7); // 7-day forecast  
    }

    // Use the FileIO function to export a complete report  
    FileIO::ExportCompleteReport(m_routes, m_wasteLocations, predictions, filename);
}

void Application::WriteRouteReport(std::ofstream& file, Route* route, const std::string& title)
{
    file << "==========================================" << std::endl;
    file << title << " (Threshold: " << route->GetWasteThreshold() << "%)" << std::endl;
    file << "==========================================" << std::endl;

    const std::vector<int>& routePath = route->GetFinalRoute();
    const std::vector<float>& distances = route->GetIndividualDistances();

    if (routePath.empty()) {
        file << "No route calculated." << std::endl;
        file << std::endl;
        return;
    }

    // Write route sequence
    file << "Route Sequence: ";
    for (size_t i = 0; i < routePath.size(); i++) {
        file << WasteLocation::dict_Id_to_Name[routePath[i]];
        if (i < routePath.size() - 1) {
            file << " -> ";
        }
    }
    file << std::endl;

    // Write individual distances
    file << "Segment Distances: ";
    for (size_t i = 0; i < distances.size(); i++) {
        file << distances[i] << " km";
        if (i < distances.size() - 1) {
            file << " + ";
        }
    }
    file << std::endl;

    // Write route costs
    file << "Total Distance: " << route->GetTotalDistance() << " km" << std::endl;
    file << "Time Taken: " << route->GetTimeTaken() << " min ("
        << route->GetTimeTaken() / 60.0f << " hours)" << std::endl;
    file << "Fuel Consumption: RM " << route->GetFuelConsumption() << std::endl;
    file << "Driver's Wage: RM " << route->GetWage() << std::endl;
    file << "Total Cost: RM " << route->GetTotalCost() << std::endl;

    file << std::endl;
}

void Application::PredictFutureWasteLevels(int daysAhead)
{
    // Update the prediction model with current waste levels
    m_wasteLevelPredictor->UpdateModel(m_wasteLocations);

    // Get predictions for demonstration purposes
    std::cout << "Waste Level Predictions for " << daysAhead << " days ahead:" << std::endl;

    for (const auto& location : m_wasteLocations) {
        if (location.GetLocationName() != "Station") {
            float currentLevel = location.GetWasteLevel();
            float predictedLevel = m_wasteLevelPredictor->PredictWasteLevel(location.GetLocationName(), daysAhead);

            std::cout << location.GetLocationName() << ": "
                << currentLevel << "% -> " << predictedLevel << "%" << std::endl;
        }
    }
}

void Application::OptimizeWithAI()
{
    // Apply AI optimizations to find the route with the lowest cost
    std::cout << "Applying AI optimization to find lowest cost route..." << std::endl;

    // Store the minimum cost and corresponding route index
    float minCost = std::numeric_limits<float>::max();
    int minCostIndex = -1;

    // Get current route index to restore if needed
    int originalRouteIndex = GetCurrentRouteIndex();

    // From the context of your code, it appears you have 5 route types
    // Non-Optimized (0), Optimized (1), MST (2), TSP (3), Greedy (4)
    const int numRoutes = 5;

    // Evaluate the cost of each route
    for (int i = 0; i < numRoutes; i++) {
        // Switch to this route to calculate costs
        SelectRoute(i);

        // Get cost of current route (if it found a valid path)
        if (m_currentRoute && m_currentRoute->GetFinalRoute().size() > 0) {
            float cost = m_currentRoute->GetTotalCost();

            // Update if this is the lowest cost so far
            if (cost < minCost) {
                minCost = cost;
                minCostIndex = i;
            }

            std::cout << "Route " << i << " (" << m_currentRoute->GetRouteName()
                << "): Cost = RM " << cost << std::endl;
        }
    }

    // Select the lowest cost route if found
    if (minCostIndex >= 0) {
        SelectRoute(minCostIndex);
        std::cout << "AI optimization complete. Selected lowest cost route: "
            << m_currentRoute->GetRouteName() << " (RM " << minCost << ")" << std::endl;
    }
    else {
        // If no valid route was found, restore original
        SelectRoute(originalRouteIndex);
        std::cout << "AI optimization failed: No valid routes found." << std::endl;
    }
}