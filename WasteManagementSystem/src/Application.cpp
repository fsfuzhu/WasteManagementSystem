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
    m_fuelCostPerKm(1.5f),          // 默认值
    m_driverWagePerHour(6.0f),     // 默认值
    m_drivingSpeedMinPerKm(1.5f)    // 默认值
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

    // 设置Application指针
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

    // Other AI component updates would go here
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
    // Open file for writing
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }

    // Write report header
    file << "==========================================" << std::endl;
    file << "Waste Management System - Route Report" << std::endl;
    file << "==========================================" << std::endl;

    // Current time
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    file << "Date: " << std::ctime(&time) << std::endl;

    // Write map
    file << "-----------------------------------------" << std::endl;
    file << "City Map:" << std::endl;
    file << "-----------------------------------------" << std::endl;

    file << "Station ------------3-------------A" << std::endl;
    file << " |                                |" << std::endl;
    file << " |                                |" << std::endl;
    file << " 4                                6" << std::endl;
    file << " |                                |" << std::endl;
    file << " |                                |" << std::endl;
    file << " |                                |" << std::endl;
    file << "G ----3-----F ----2----C ----5----B" << std::endl;
    file << " |           |                    " << std::endl;
    file << " |           |                    " << std::endl;
    file << " |           |                    " << std::endl;
    file << " |           2----D ----4         " << std::endl;
    file << std::endl;

    // Write waste levels
    file << "-----------------------------------------" << std::endl;
    file << "Waste Levels:" << std::endl;
    file << "-----------------------------------------" << std::endl;

    file << "Location\tWaste Level" << std::endl;
    file << "-----------------------------------------" << std::endl;

    for (const auto& location : m_wasteLocations) {
        file << location.GetLocationName() << "\t\t"
            << location.GetWasteLevel() << "%" << std::endl;
    }

    file << std::endl;

    // Write route information for each algorithm
    WriteRouteReport(file, m_nonOptimizedRoute.get(), "Non-Optimized Route");
    WriteRouteReport(file, m_optimizedRoute.get(), "Optimized Route");
    WriteRouteReport(file, m_mstRoute.get(), "MST Route");
    WriteRouteReport(file, m_tspRoute.get(), "TSP Route");
    WriteRouteReport(file, m_greedyRoute.get(), "Greedy Route");

    // Write comparison summary
    file << "==========================================" << std::endl;
    file << "Route Comparison Summary:" << std::endl;
    file << "==========================================" << std::endl;

    file << "Route Type\tTotal Distance\tTotal Cost" << std::endl;
    file << "-----------------------------------------" << std::endl;

    file << "Non-Optimized\t" << m_nonOptimizedRoute->GetTotalDistance() << " km\t"
        << "RM " << m_nonOptimizedRoute->GetTotalCost() << std::endl;

    file << "Optimized\t" << m_optimizedRoute->GetTotalDistance() << " km\t"
        << "RM " << m_optimizedRoute->GetTotalCost() << std::endl;

    file << "MST\t\t" << m_mstRoute->GetTotalDistance() << " km\t"
        << "RM " << m_mstRoute->GetTotalCost() << std::endl;

    file << "TSP\t\t" << m_tspRoute->GetTotalDistance() << " km\t"
        << "RM " << m_tspRoute->GetTotalCost() << std::endl;

    file << "Greedy\t\t" << m_greedyRoute->GetTotalDistance() << " km\t"
        << "RM " << m_greedyRoute->GetTotalCost() << std::endl;

    file << std::endl;

    // Write AI predictions
    file << "==========================================" << std::endl;
    file << "Waste Level Predictions:" << std::endl;
    file << "==========================================" << std::endl;

    file << "Location\tCurrent Level\t3-Day Forecast\t7-Day Forecast" << std::endl;
    file << "-----------------------------------------" << std::endl;

    for (const auto& location : m_wasteLocations) {
        if (location.GetLocationName() != "Station") {
            file << location.GetLocationName() << "\t\t"
                << location.GetWasteLevel() << "%\t\t"
                << m_wasteLevelPredictor->PredictWasteLevel(location.GetLocationName(), 3) << "%\t\t"
                << m_wasteLevelPredictor->PredictWasteLevel(location.GetLocationName(), 7) << "%" << std::endl;
        }
    }

    file << std::endl;

    // Close file
    file.close();

    std::cout << "Route report exported to: " << filename << std::endl;
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
    // Apply AI optimizations to find the best route strategy
    std::cout << "Applying AI optimization..." << std::endl;

    // This is a placeholder for actual AI optimization
    // In a real implementation, we would use the AI components to determine
    // the best route strategy based on current conditions

    // For demonstration, select the optimized route
    SelectRoute(1);

    std::cout << "AI optimization complete. Selected: " << m_currentRoute->GetRouteName() << std::endl;
}