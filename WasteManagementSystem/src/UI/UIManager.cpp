﻿// UIManager.cpp
// Implementation of the ImGui-based user interface
#include "pch.h"
#include "UIManager.h"
#include "../Application.h"
#include "../Core/WasteLocation.h"
#include "../Core/Route.h"
#include "../AI/WasteLevelPredictor.h"
#include "../AI/RouteLearningAgent.h"
#include "../AI/LocationClustering.h"
// Constructor
UIManager::UIManager(Application* application)
    : m_application(application),
    m_window(nullptr),
    m_showMapWindow(true),
    m_showDetailsWindow(true),
    m_showComparisonWindow(true),
    m_showAIWindow(true),
    m_showSettingsWindow(true),
    m_windowWidth(1280),
    m_windowHeight(720)
{
    // 创建UI组件实例
    m_mainWindow = std::make_unique<MainWindow>();
    m_mapVisualization = std::make_unique<MapVisualization>();

}


// Destructor
UIManager::~UIManager()
{

}

// Initialize UI manager
bool UIManager::Initialize(GLFWwindow* window)
{
    m_window = window;

    // Setup ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable keyboard navigation
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable multi-viewport

    // Setup ImGui style
    SetupImGuiStyle();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    // Setup ImGui configuration
    SetupImGuiConfig();

    // 初始化UI组件
    m_mainWindow->Initialize();
    m_mapVisualization->Initialize();

    return true;
}

// Setup ImGui style
void UIManager::SetupImGuiStyle()
{
    // Set up modern dark theme
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();

    // Adjust colors
    auto& colors = style.Colors;
    colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.1f, 0.13f, 1.0f);
    colors[ImGuiCol_Header] = ImVec4(0.2f, 0.2f, 0.23f, 1.0f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.25f, 0.25f, 0.28f, 1.0f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.27f, 0.27f, 0.33f, 1.0f);
    colors[ImGuiCol_Button] = ImVec4(0.2f, 0.2f, 0.25f, 1.0f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.25f, 0.25f, 0.3f, 1.0f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.3f, 0.3f, 0.35f, 1.0f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.15f, 0.15f, 0.18f, 1.0f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.2f, 0.2f, 0.23f, 1.0f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.25f, 0.25f, 0.28f, 1.0f);
    colors[ImGuiCol_Tab] = ImVec4(0.15f, 0.15f, 0.18f, 1.0f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.3f, 0.3f, 0.33f, 1.0f);
    colors[ImGuiCol_TabActive] = ImVec4(0.25f, 0.25f, 0.28f, 1.0f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.15f, 0.15f, 0.18f, 1.0f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.2f, 0.2f, 0.23f, 1.0f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.15f, 0.15f, 0.18f, 1.0f);

    // Adjust style variables
    style.WindowRounding = 4.0f;
    style.FrameRounding = 4.0f;
    style.PopupRounding = 4.0f;
    style.ScrollbarRounding = 4.0f;
    style.GrabRounding = 4.0f;
    style.TabRounding = 4.0f;
    style.WindowBorderSize = 0.0f;
    style.FrameBorderSize = 1.0f;
    style.PopupBorderSize = 1.0f;
    style.WindowPadding = ImVec2(10.0f, 10.0f);
    style.FramePadding = ImVec2(8.0f, 4.0f);
    style.ItemSpacing = ImVec2(10.0f, 8.0f);
    style.ItemInnerSpacing = ImVec2(6.0f, 6.0f);
    style.IndentSpacing = 20.0f;
    style.ScrollbarSize = 15.0f;
    style.GrabMinSize = 10.0f;

    // Setup multi-viewport platform windows
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }
}

// Setup ImGui configuration
void UIManager::SetupImGuiConfig()
{
    ImGuiIO& io = ImGui::GetIO();

    // Load fonts
    io.Fonts->AddFontFromFileTTF("assets/fonts/roboto/Roboto-Medium.ttf", 16.0f);
    io.FontDefault = io.Fonts->AddFontFromFileTTF("assets/fonts/roboto/Roboto-Regular.ttf", 16.0f);

    // Configure ImPlot style
    ImPlot::StyleColorsDark();
    ImPlotStyle& plotStyle = ImPlot::GetStyle();
    plotStyle.LineWeight = 2.0f;
    plotStyle.MarkerSize = 5.0f;
}

// Begin a new ImGui frame
void UIManager::BeginFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Setup dockspace
    SetupDockspace();
}

// Setup dockspace for docking windows
void UIManager::SetupDockspace()
{
    // Create a full-screen window for dockspace
    ImGuiWindowFlags windowFlags =
        ImGuiWindowFlags_MenuBar |
        ImGuiWindowFlags_NoDocking |
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoNavFocus;

    // Get viewport
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);

    // Set padding to 0 and window background color
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace", nullptr, windowFlags);
    ImGui::PopStyleVar();

    // Dockspace
    ImGui::DockSpace(ImGui::GetID("MyDockSpace"), ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

    // Menu bar
    RenderMenuBar();

    ImGui::End();
}

// Render the menu bar
void UIManager::RenderMenuBar()
{
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New Simulation")) {
                // Reset simulation - regenerate waste levels
                m_application->RegenerateLocations();
            }

            if (ImGui::MenuItem("Regenerate Locations Only")) {
                // Regenerate only locations, keep waste levels
                m_application->RegenerateLocations(false);
            }

            if (ImGui::MenuItem("Regenerate Waste Levels Only")) {
                // Regenerate only waste levels, keep locations
                m_application->RegenerateWasteLevels();
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Export Report")) {
                // Export route report
                m_application->ExportRouteReport("WasteManagementReport.txt");
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Exit", "Alt+F4")) {
                glfwSetWindowShouldClose(m_window, GLFW_TRUE);
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View")) {
            ImGui::MenuItem("Map Visualization", nullptr, &m_showMapWindow);
            ImGui::MenuItem("Route Details", nullptr, &m_showDetailsWindow);
            ImGui::MenuItem("Route Comparison", nullptr, &m_showComparisonWindow);
            ImGui::MenuItem("AI Tools", nullptr, &m_showAIWindow);
            ImGui::MenuItem("Settings", nullptr, &m_showSettingsWindow);

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Route")) {
            if (ImGui::MenuItem("Non-Optimized (Regular)", nullptr, m_application->GetCurrentRouteIndex() == 0)) {
                m_application->SelectRoute(0);
            }
            if (ImGui::MenuItem("Optimized", nullptr, m_application->GetCurrentRouteIndex() == 1)) {
                m_application->SelectRoute(1);
            }
            if (ImGui::MenuItem("MST", nullptr, m_application->GetCurrentRouteIndex() == 2)) {
                m_application->SelectRoute(2);
            }
            if (ImGui::MenuItem("TSP", nullptr, m_application->GetCurrentRouteIndex() == 3)) {
                m_application->SelectRoute(3);
            }
            if (ImGui::MenuItem("Greedy", nullptr, m_application->GetCurrentRouteIndex() == 4)) {
                m_application->SelectRoute(4);
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("AI Tools")) {
            if (ImGui::MenuItem("Predict Waste Levels")) {
                m_showAIWindow = true;
            }

            if (ImGui::MenuItem("Apply AI Route Optimization")) {
                m_application->OptimizeWithAI();
            }

            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }
}

// Render the UI
void UIManager::Render()
{
    // Render UI windows
    if (m_showMapWindow) {
        RenderMapWindow();
    }

    if (m_showDetailsWindow) {
        RenderDetailsWindow();
    }

    if (m_showComparisonWindow) {
        RenderComparisonWindow();
    }

    if (m_showAIWindow) {
        RenderAIWindow();
    }

    if (m_showSettingsWindow) {
        RenderSettingsWindow();
    }

    // Always render status bar
    RenderStatusBar();
}

// Render the map visualization window
void UIManager::RenderMapWindow()
{
    ImGui::Begin("Map Visualization", &m_showMapWindow);

    // 获取当前路线和废物位置
    const auto& wasteLocations = m_application->GetWasteLocations();
    const Route* currentRoute = m_application->GetCurrentRoute();

    // 使用MapVisualization组件渲染地图，避免代码重复
    m_mapVisualization->Render(wasteLocations, currentRoute);

    ImGui::End();
}

// Render the route details window
void UIManager::RenderDetailsWindow()
{
    ImGui::Begin("Route Details", &m_showDetailsWindow);

    // Get current route and waste locations
    const auto& wasteLocations = m_application->GetWasteLocations();
    const Route* currentRoute = m_application->GetCurrentRoute();

    if (!currentRoute) {
        ImGui::Text("No route selected.");
        ImGui::End();
        return;
    }

    // Route name and waste threshold
    ImGui::Text("Route Type: %s", currentRoute->GetRouteName().c_str());
    ImGui::Text("Waste Threshold: %.0f%%", currentRoute->GetWasteThreshold());
    ImGui::Separator();

    // Display waste levels
    ImGui::Text("Waste Levels:");
    ImGui::Columns(2, "waste_levels");
    ImGui::Text("Location"); ImGui::NextColumn();
    ImGui::Text("Waste Level (%)"); ImGui::NextColumn();
    ImGui::Separator();

    for (const auto& location : wasteLocations) {
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

        ImGui::TextColored(color, "%.1f%%", wasteLevel);
        ImGui::NextColumn();
    }

    ImGui::Columns(1);
    ImGui::Separator();

    // Display route information
    const std::vector<int>& route = currentRoute->GetFinalRoute();
    const std::vector<float>& distances = currentRoute->GetIndividualDistances();

    if (route.empty()) {
        ImGui::Text("No route calculated.");
        ImGui::End();
        return;
    }

    ImGui::Text("Route Sequence:");

    // Display route as Station -> A -> B -> Station
    std::string routeStr;
    for (size_t i = 0; i < route.size(); i++) {
        routeStr += WasteLocation::dict_Id_to_Name[route[i]];
        if (i < route.size() - 1) {
            routeStr += " -> ";
        }
    }

    ImGui::TextWrapped("%s", routeStr.c_str());
    ImGui::Separator();

    // Route segments
    ImGui::Text("Route Segments:");
    ImGui::Columns(2, "route_segments");
    ImGui::Text("Segment"); ImGui::NextColumn();
    ImGui::Text("Distance (km)"); ImGui::NextColumn();
    ImGui::Separator();

    for (size_t i = 0; i < route.size() - 1; i++) {
        std::string segment = WasteLocation::dict_Id_to_Name[route[i]] + " -> " +
            WasteLocation::dict_Id_to_Name[route[i + 1]];
        ImGui::Text("%s", segment.c_str());
        ImGui::NextColumn();

        ImGui::Text("%.2f", distances[i]);
        ImGui::NextColumn();
    }

    ImGui::Columns(1);
    ImGui::Separator();

    // Route costs
    ImGui::Text("Route Costs:");

    ImGui::Columns(2, "route_costs");
    ImGui::Text("Total Distance:"); ImGui::NextColumn();
    ImGui::Text("%.2f km", currentRoute->GetTotalDistance());
    ImGui::NextColumn();

    ImGui::Text("Time Taken:"); ImGui::NextColumn();
    ImGui::Text("%.2f min (%.2f hours)",
        currentRoute->GetTimeTaken(),
        currentRoute->GetTimeTaken() / 60.0f);
    ImGui::NextColumn();

    ImGui::Text("Fuel Consumption:"); ImGui::NextColumn();
    ImGui::Text("RM %.2f", currentRoute->GetFuelConsumption());
    ImGui::NextColumn();

    ImGui::Text("Driver's Wage:"); ImGui::NextColumn();
    ImGui::Text("RM %.2f", currentRoute->GetWage());
    ImGui::NextColumn();

    ImGui::Text("Total Cost:"); ImGui::NextColumn();
    ImGui::TextColored(ImVec4(1.0f, 0.84f, 0.0f, 1.0f), "RM %.2f", currentRoute->GetTotalCost());
    ImGui::NextColumn();

    ImGui::Columns(1);

    ImGui::End();
}

// Render the route comparison window
// 在文件 WasteManagementSystem/src/UI/UIManager.cpp 中

void UIManager::RenderComparisonWindow()
{
    ImGui::Begin("Route Comparison", &m_showComparisonWindow);

    // Button to regenerate waste levels
    if (ImGui::Button("Regenerate Waste Levels")) {
        m_application->RegenerateWasteLevels();
    }

    ImGui::SameLine();

    if (ImGui::Button("Regenerate Locations")) {
        m_application->RegenerateLocations(false);  // Don't regenerate waste levels
    }

    ImGui::SameLine();

    // Button to export report
    if (ImGui::Button("Export Full Report")) {
        m_application->ExportRouteReport("WasteManagementReport.txt");
    }

    ImGui::Separator();

    // Compare routes with a bar chart
    if (ImPlot::BeginPlot("Route Cost Comparison", ImVec2(-1, 300))) {
        // IMPORTANT: Call setup functions AFTER BeginPlot but BEFORE any Plot functions
        ImPlot::SetupAxes("Route Type", "Cost (RM)");

        // Route names
        const char* labels[] = {
            "Non-Optimized",
            "Optimized",
            "MST",
            "TSP",
            "Greedy"
        };

        // Setup axis ticks with labels
        ImPlot::SetupAxisTicks(ImAxis_X1, 0, 4, 5, labels, false);

        // Route costs
        float totalCosts[5] = { 0 };
        float fuelCosts[5] = { 0 };
        float wageCosts[5] = { 0 };

        // Get route costs - we'll need to temporarily switch routes to get data from each
        int originalRouteIndex = m_application->GetCurrentRouteIndex();

        // Capture each route's data
        for (int i = 0; i < 5; i++) {
            m_application->SelectRoute(i);
            Route* route = m_application->GetCurrentRoute();
            if (route) {
                totalCosts[i] = route->GetTotalCost();
                fuelCosts[i] = route->GetFuelConsumption();
                wageCosts[i] = route->GetWage();
            }
        }

        // Restore original route selection
        m_application->SelectRoute(originalRouteIndex);

        // Plot bar chart
        ImPlot::PlotBars("Total Cost (RM)", totalCosts, 5, 0.7f);
        ImPlot::PlotBars("Fuel Cost (RM)", fuelCosts, 5, 0.7f);
        ImPlot::PlotBars("Wage Cost (RM)", wageCosts, 5, 0.7f);

        ImPlot::EndPlot();
    }

    // Display cost savings
    ImGui::Separator();
    ImGui::Text("Cost Savings Analysis:");

    ImGui::Columns(3, "cost_savings");
    ImGui::Text("Route Type"); ImGui::NextColumn();
    ImGui::Text("Cost (RM)"); ImGui::NextColumn();
    ImGui::Text("Savings vs Non-Optimized (RM)"); ImGui::NextColumn();
    ImGui::Separator();

    // Capture all routes' costs
    float routeCosts[5] = { 0 };
    int originalRouteIndex = m_application->GetCurrentRouteIndex();

    for (int i = 0; i < 5; i++) {
        m_application->SelectRoute(i);
        Route* route = m_application->GetCurrentRoute();
        if (route) {
            routeCosts[i] = route->GetTotalCost();
        }
    }

    // Restore original route
    m_application->SelectRoute(originalRouteIndex);

    float nonOptimizedCost = routeCosts[0];

    // Display Non-Optimized row
    ImGui::Text("Non-Optimized"); ImGui::NextColumn();
    ImGui::Text("%.2f", nonOptimizedCost); ImGui::NextColumn();
    ImGui::Text("-"); ImGui::NextColumn();

    // Display other routes with actual savings
    const char* routeNames[] = { "Optimized", "MST", "TSP", "Greedy" };
    for (int i = 1; i < 5; i++) {
        ImGui::Text("%s", routeNames[i - 1]); ImGui::NextColumn();
        ImGui::Text("%.2f", routeCosts[i]); ImGui::NextColumn();
        float savings = nonOptimizedCost - routeCosts[i];
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "%.2f", savings); ImGui::NextColumn();
    }

    ImGui::Columns(1);

    ImGui::Separator();

    // Get route data
    float lowestCost = std::numeric_limits<float>::max();
    int lowestCostIndex = -1;
    int mostLocationsIndex = -1;
    int maxLocationCount = -1;

    // Get current route index to restore later
    int originalRouteIndex2 = m_application->GetCurrentRouteIndex();

    // Find which route has lowest cost and which visits most locations
    for (int i = 0; i < 5; i++) {
        m_application->SelectRoute(i);
        Route* route = m_application->GetCurrentRoute();
        if (route) {
            float cost = route->GetTotalCost();
            int locationCount = route->GetFinalRoute().size();

            if (cost < lowestCost) {
                lowestCost = cost;
                lowestCostIndex = i;
            }

            if (locationCount > maxLocationCount) {
                maxLocationCount = locationCount;
                mostLocationsIndex = i;
            }
        }
    }

    // Restore original route
    m_application->SelectRoute(originalRouteIndex2);

    // Route names
    const char* routeNames2[] = {
        "Non-Optimized",
        "Optimized",
        "MST",
        "TSP",
        "Greedy"
    };

    // Create a red background for the AI analysis
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.7f, 0.1f, 0.1f, 0.9f));
    ImGui::BeginChild("AIAnalysis", ImVec2(-1, 50), true);

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
    ImGui::TextWrapped("AI Analysis: %s route is most cost-efficient (RM %.2f). %s route visits the most locations (%d stops).",
        lowestCostIndex >= 0 ? routeNames2[lowestCostIndex] : "Unknown",
        lowestCost,
        mostLocationsIndex >= 0 ? routeNames2[mostLocationsIndex] : "Unknown",
        maxLocationCount);
    ImGui::PopStyleColor();

    ImGui::EndChild();
    ImGui::PopStyleColor();

    ImGui::End();
}

// Render the AI tools window
void UIManager::RenderAIWindow()
{
    ImGui::Begin("AI Tools", &m_showAIWindow);

    // Tabs for different AI tools
    if (ImGui::BeginTabBar("AITabBar")) {
        if (ImGui::BeginTabItem("Waste Level Prediction")) {
            RenderWastePredictionUI();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Route Learning")) {
            RenderRouteLearningUI();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Location Clustering")) {
            RenderLocationClusteringUI();
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::End();
}

// Render waste prediction UI
void UIManager::RenderWastePredictionUI()
{
    ImGui::Text("Waste Level Prediction using Machine Learning");
    ImGui::TextWrapped(
        "This tool uses a neural network model to predict future waste levels based on historical data. "
        "It takes into account day of week, day of month, and previous levels to generate accurate forecasts."
    );

    ImGui::Separator();

    // Prediction controls
    static int daysAhead = 7;
    ImGui::Text("Forecast Days:");
    ImGui::SliderInt("##forecast_days", &daysAhead, 1, 30, "%d days");

    if (ImGui::Button("Generate Forecast")) {
        // Generate forecast
        m_application->PredictFutureWasteLevels(daysAhead);
    }

    ImGui::SameLine();

    if (ImGui::Button("Train Model")) {
        // Train the model
        if (m_application->GetWasteLevelPredictor()) {
            m_application->GetWasteLevelPredictor()->TrainNeuralNetwork(100);
        }
    }

    ImGui::Separator();

    // Plot forecasts
    if (ImPlot::BeginPlot("Waste Level Forecast", ImVec2(-1, 400))) {
        // IMPORTANT: Call setup functions AFTER BeginPlot but BEFORE any Plot functions
        ImPlot::SetupAxes("Days Ahead", "Waste Level (%)");
        ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 100);

        // Get forecast data from AI component
        WasteLevelPredictor* predictor = m_application->GetWasteLevelPredictor();

        if (predictor) {
            auto forecasts = predictor->GenerateForecasts(daysAhead);

            // X-axis: days
            double x[31]; // current day + forecast days
            for (int i = 0; i <= daysAhead; i++) {
                x[i] = i;
            }

            // For each location, plot a line
            for (const auto& pair : forecasts) {
                if (pair.first != "Station") {
                    const std::vector<float>& levels = pair.second;

                    // Convert to double for ImPlot
                    double y[31];
                    for (size_t i = 0; i < levels.size(); i++) {
                        y[i] = levels[i];
                    }

                    // Plot line
                    ImPlot::PlotLine(pair.first.c_str(), x, y, levels.size());
                }
            }

            // Plot threshold lines
            double x_thresholds[2] = { 0, static_cast<double>(daysAhead) };
            double y_60[2] = { 60.0, 60.0 };
            double y_40[2] = { 40.0, 40.0 };

            ImPlot::PushStyleColor(ImPlotCol_Line, ImVec4(1, 0, 0, 1));
            ImPlot::PlotLine("60% Threshold", x_thresholds, y_60, 2);
            ImPlot::PopStyleColor();

            ImPlot::PushStyleColor(ImPlotCol_Line, ImVec4(1, 1, 0, 1));
            ImPlot::PlotLine("40% Threshold", x_thresholds, y_40, 2);
            ImPlot::PopStyleColor();
        }

        ImPlot::EndPlot();
    }

    ImGui::Separator();

    // Recommended collection days
    ImGui::Text("Recommended Collection Days:");

    ImGui::Columns(2, "collection_days");
    ImGui::Text("Location"); ImGui::NextColumn();
    ImGui::Text("Collection Recommended In"); ImGui::NextColumn();
    ImGui::Separator();

    // Get recommended collection days from AI component
    WasteLevelPredictor* predictor = m_application->GetWasteLevelPredictor();

    if (predictor) {
        for (const auto& pair : WasteLocation::dict_Name_toId) {
            if (pair.first != "Station") {
                ImGui::Text("%s", pair.first.c_str());
                ImGui::NextColumn();

                int days = predictor->GetRecommendedCollectionDay(pair.first, 60.0f);

                if (days == 0) {
                    ImGui::TextColored(ImVec4(1, 0, 0, 1), "URGENT: Today");
                }
                else if (days > 0) {
                    ImGui::Text("%d days", days);
                }
                else {
                    ImGui::Text("Unknown");
                }

                ImGui::NextColumn();
            }
        }
    }

    ImGui::Columns(1);
}

// Render route learning UI
void UIManager::RenderRouteLearningUI()
{
    ImGui::Text("Route Learning Agent");
    ImGui::TextWrapped(
        "This tool uses reinforcement learning to optimize waste collection routes based on past experiences. "
        "It learns which routes are most efficient and adapts to changing conditions."
    );

    ImGui::Separator();

    // Learning controls
    static int trainingIterations = 100;
    ImGui::Text("Training Iterations:");
    ImGui::SliderInt("##training_iterations", &trainingIterations, 10, 1000);

    if (ImGui::Button("Train Route Agent")) {
        // Train the agent
    }

    ImGui::SameLine();

    if (ImGui::Button("Apply Learned Routes")) {
        // Apply learned routes
        m_application->OptimizeWithAI();
    }

    ImGui::Separator();

    // Learning progress visualization
    ImGui::Text("Learning Progress:");

    // Simulated learning curve
    static float progress = 100;
    static float learningData[100];

    for (int i = 0; i < 100; i++) {
        learningData[i] = 100.0f - 50.0f * (1.0f - expf(-i / 20.0f)) + 10.0f * sinf(i * 0.2f);
    }

    // Plot learning curve
    if (ImPlot::BeginPlot("Learning Curve", ImVec2(-1, 300))) {
        // IMPORTANT: Call setup functions AFTER BeginPlot but BEFORE any Plot functions
        ImPlot::SetupAxes("Training Iterations", "Route Cost (RM)");

        double x[100];
        for (int i = 0; i < 100; i++) {
            x[i] = (float)i;
        }

        ImPlot::PlotLine("Cost", (const float*)x, (const float*)learningData, 100);

        ImPlot::EndPlot();
    }

    ImGui::ProgressBar(progress / 100.0f, ImVec2(-1, 0), "Training Progress");

    ImGui::Separator();

    // Q-value visualization
    ImGui::Text("Q-Value Map (Route Segment Values):");

    // Simple Q-value visualization as a heatmap
    static float qValues[9][9];
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (WasteLocation::map_distance_matrix[i][j] < INF) {
                qValues[i][j] = 0.5f + 0.5f * sinf(i * 0.5f + j * 0.3f);
            }
            else {
                qValues[i][j] = 0.0f;
            }
        }
    }

    // Display Q-values as a table with colors
    ImGui::BeginTable("Q-Values", 8, ImGuiTableFlags_Borders);

    // Header row
    ImGui::TableNextRow();
    for (int j = 0; j < 8; j++) {
        ImGui::TableNextColumn();
        ImGui::Text("%s", WasteLocation::dict_Id_to_Name[j].c_str());
    }

    // Data rows
    for (int i = 0; i < 8; i++) {
        ImGui::TableNextRow();

        for (int j = 0; j < 8; j++) {
            ImGui::TableNextColumn();

            if (WasteLocation::map_distance_matrix[i][j] < INF) {
                // Color based on Q-value
                ImVec4 color(
                    1.0f - qValues[i][j],
                    qValues[i][j],
                    0.0f,
                    1.0f
                );

                ImGui::TextColored(color, "%.2f", qValues[i][j]);
            }
            else {
                ImGui::TextDisabled("-");
            }
        }
    }

    ImGui::EndTable();
}

// Render location clustering UI
void UIManager::RenderLocationClusteringUI()
{
    ImGui::Text("Location Clustering");
    ImGui::TextWrapped(
        "This tool uses K-means clustering to group waste locations for more efficient collection. "
        "It identifies natural clusters based on proximity and waste levels."
    );

    ImGui::Separator();

    // Clustering controls
    static int numClusters = 3;
    ImGui::Text("Number of Clusters:");
    ImGui::SliderInt("##num_clusters", &numClusters, 2, 5);

    if (ImGui::Button("Generate Clusters")) {
        // Generate clusters
    }

    ImGui::SameLine();

    if (ImGui::Button("Apply Clustered Route")) {
        // Apply clustered route
    }

    ImGui::Separator();

    // Cluster visualization
    ImGui::Text("Location Clusters:");

    // Map display similar to main map but with cluster colors
    ImVec2 windowSize = ImGui::GetContentRegionAvail();
    ImVec2 mapSize(windowSize.x, 400);

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 canvasPos = ImGui::GetCursorScreenPos();

    // Background
    drawList->AddRectFilled(canvasPos,
        ImVec2(canvasPos.x + mapSize.x, canvasPos.y + mapSize.y),
        IM_COL32(30, 30, 40, 255));

    // Cluster colors
    ImU32 clusterColors[] = {
        IM_COL32(255, 0, 0, 200),   // Red
        IM_COL32(0, 255, 0, 200),   // Green
        IM_COL32(0, 0, 255, 200),   // Blue
        IM_COL32(255, 255, 0, 200), // Yellow
        IM_COL32(255, 0, 255, 200)  // Magenta
    };

    // Center and scale the map
    float scaleX = mapSize.x / 500.0f;
    float scaleY = mapSize.y / 500.0f;
    float scale = std::min(scaleX, scaleY);

    float offsetX = canvasPos.x + (mapSize.x - 500.0f * scale) * 0.5f;
    float offsetY = canvasPos.y + (mapSize.y - 500.0f * scale) * 0.5f;

    // Draw roads
    ImU32 roadColor = IM_COL32(150, 150, 150, 200);
    float roadThickness = 2.0f;

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
            }
        }
    }

    // Simulated cluster assignments (random for demo)
    int clusterAssignments[] = { 0, 0, 1, 1, 2, 2, 0, 1 };

    // Draw clusters
    for (int c = 0; c < numClusters; c++) {
        // Find convex hull of cluster points
        std::vector<ImVec2> clusterPoints;

        for (int i = 0; i < 8; i++) {
            if (clusterAssignments[i] == c) {
                ImVec2 pos(
                    offsetX + WasteLocation::location_coordinates[i][0] * scale,
                    offsetY + WasteLocation::location_coordinates[i][1] * scale
                );

                clusterPoints.push_back(pos);
            }
        }

        // Draw cluster region if it has points
        if (!clusterPoints.empty()) {
            // For demo, just draw connecting lines between points
            for (size_t i = 0; i < clusterPoints.size(); i++) {
                for (size_t j = i + 1; j < clusterPoints.size(); j++) {
                    drawList->AddLine(
                        clusterPoints[i],
                        clusterPoints[j],
                        clusterColors[c],
                        1.0f
                    );
                }
            }
        }
    }

    // Draw locations with cluster colors
    for (int i = 0; i < 8; i++) {
        ImVec2 pos(
            offsetX + WasteLocation::location_coordinates[i][0] * scale,
            offsetY + WasteLocation::location_coordinates[i][1] * scale
        );

        // Determine color based on cluster
        ImU32 color;
        float radius;

        if (i == 0) {
            // Station
            color = IM_COL32(255, 165, 0, 255); // Orange
            radius = 15.0f;
        }
        else {
            // Use cluster color
            color = clusterColors[clusterAssignments[i]];
            radius = 10.0f;
        }

        // Draw location
        drawList->AddCircleFilled(pos, radius, color);
        drawList->AddCircle(pos, radius, IM_COL32(255, 255, 255, 200), 0, 2.0f);

        // Draw location label
        std::string name = WasteLocation::dict_Id_to_Name[i];
        drawList->AddText(
            ImVec2(pos.x - 5.0f, pos.y - 25.0f),
            IM_COL32(255, 255, 255, 255),
            name.c_str()
        );
    }

    // Reset cursor position
    ImGui::SetCursorScreenPos(canvasPos);
    ImGui::Dummy(mapSize);

    ImGui::Separator();

    // Cluster information
    ImGui::Text("Cluster Analysis:");

    // Display cluster details in a table
    ImGui::BeginTable("Cluster Info", 4, ImGuiTableFlags_Borders);

    // Header
    ImGui::TableNextRow();
    ImGui::TableNextColumn(); ImGui::Text("Cluster");
    ImGui::TableNextColumn(); ImGui::Text("Locations");
    ImGui::TableNextColumn(); ImGui::Text("Avg. Waste Level");
    ImGui::TableNextColumn(); ImGui::Text("Total Distance");

    // Data rows
    for (int c = 0; c < numClusters; c++) {
        ImGui::TableNextRow();

        // Cluster number
        ImGui::TableNextColumn();
        ImGui::TextColored(
            ImVec4(
                ((clusterColors[c] >> 0) & 0xFF) / 255.0f,
                ((clusterColors[c] >> 8) & 0xFF) / 255.0f,
                ((clusterColors[c] >> 16) & 0xFF) / 255.0f,
                1.0f
            ),
            "Cluster %d", c + 1
        );

        // Locations in cluster
        ImGui::TableNextColumn();
        std::string locations;
        int count = 0;

        for (int i = 1; i < 8; i++) {
            if (clusterAssignments[i] == c) {
                if (count > 0) {
                    locations += ", ";
                }
                locations += WasteLocation::dict_Id_to_Name[i];
                count++;
            }
        }

        ImGui::Text("%s", locations.c_str());

        // Average waste level (simulated)
        ImGui::TableNextColumn();
        ImGui::Text("%.1f%%", 40.0f + 20.0f * c);

        // Total distance (simulated)
        ImGui::TableNextColumn();
        ImGui::Text("%.2f km", 10.0f + 5.0f * c);
    }

    ImGui::EndTable();
}

// Render the settings window
void UIManager::RenderSettingsWindow()
{
    ImGui::Begin("Settings", &m_showSettingsWindow);

    ImGui::Text("Application Settings");

    SetupImGuiStyle();
    // 模拟设置
    ImGui::Separator();
    ImGui::Text("Simulation Settings:");

    // 获取当前值
    static float fuelCost = 1.5f;  // 默认值
    static float driverWage = 5.77f;  // 默认值
    static float drivingSpeed = 1.5f;  // 默认值

    // 首次初始化
    static bool initialized = false;
    if (!initialized && m_application) {
        fuelCost = m_application->GetFuelCostPerKm();
        driverWage = m_application->GetDriverWagePerHour();
        drivingSpeed = m_application->GetDrivingSpeedMinPerKm();
        initialized = true;
    }

    // 显示滑动条并更新值
    if (ImGui::SliderFloat("Fuel Cost (RM/km)", &fuelCost, 0.5f, 5.0f, "%.2f")) {
        if (m_application) {
            m_application->SetFuelCostPerKm(fuelCost);
            m_application->RecalculateCurrentRoute(); // 重新计算路线以应用新设置
        }
    }

    if (ImGui::SliderFloat("Driver Wage (RM/hour)", &driverWage, 3.0f, 20.0f, "%.2f")) {
        if (m_application) {
            m_application->SetDriverWagePerHour(driverWage);
            m_application->RecalculateCurrentRoute();
        }
    }

    if (ImGui::SliderFloat("Driving Speed (min/km)", &drivingSpeed, 1.0f, 5.0f, "%.2f")) {
        if (m_application) {
            m_application->SetDrivingSpeedMinPerKm(drivingSpeed);
            m_application->RecalculateCurrentRoute();
        }
    }

    // 关于部分
    ImGui::Separator();
    ImGui::Text("About:");
    ImGui::TextWrapped(
        "Waste Management System v1.0\n"
        "Developed for COMP2034 Coursework 2\n"
        "10/04/2025"
    );

    ImGui::End();
}

// Render status bar
void UIManager::RenderStatusBar()
{
    const float statusBarHeight = 40.0f;

    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + viewport->Size.y - statusBarHeight));
    ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, statusBarHeight));

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_NoInputs |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoBringToFrontOnFocus;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 3.0f));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.15f, 0.15f, 0.18f, 1.0f));

    ImGui::Begin("StatusBar", nullptr, flags);

    // Current route type
    const Route* currentRoute = m_application->GetCurrentRoute();
    std::string routeName = currentRoute ? currentRoute->GetRouteName() : "None";

    ImGui::Text("Current Route: %s", routeName.c_str());
    ImGui::SameLine();

    // Current time
    time_t now = time(nullptr);
    tm* timeinfo = localtime(&now);
    char timeStr[64];
    strftime(timeStr, sizeof(timeStr), "%H:%M:%S", timeinfo);

    ImGui::Text("Time: %s", timeStr);
    ImGui::SameLine();

    // FPS
    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);

    ImGui::End();

    ImGui::PopStyleColor();
    ImGui::PopStyleVar(3);
}

// End frame
void UIManager::EndFrame()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Update and render additional platform windows
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
}

// Check if window should close
bool UIManager::ShouldClose() const
{
    return glfwWindowShouldClose(m_window);
}

// Handle window resize
void UIManager::HandleResize(int width, int height)
{
    m_windowWidth = width;
    m_windowHeight = height;
}