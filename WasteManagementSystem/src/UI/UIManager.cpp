// UIManager.cpp
// Implementation of the ImGui-based user interface
#include "pch.h"
#include "UIManager.h"
#include "..\Application.h"

// Constructor
UIManager::UIManager(Application* application)
    : m_application(application),
    m_window(nullptr),
    m_showMapWindow(true),
    m_showDetailsWindow(true),
    m_showComparisonWindow(true),
    m_showAIWindow(true),
    m_showSettingsWindow(false),
    m_darkTheme(true),
    m_fontScale(1.0f),
    m_windowWidth(1280),
    m_windowHeight(720)
{
    // 创建UI组件实例
    m_mainWindow = std::make_unique<MainWindow>();
    m_mapVisualization = std::make_unique<MapVisualization>();
    m_routeComparisonPanel = std::make_unique<RouteComparisonPanel>();
    m_routeDetailsPanel = std::make_unique<RouteDetailsPanel>();
    m_aiToolsPanel = std::make_unique<AIToolsPanel>();

    // 设置Panel需要传入Theme Toggle回调
    m_settingsPanel = std::make_unique<SettingsPanel>(
        [this]() { this->ToggleTheme(); }
    );
}

// Destructor
UIManager::~UIManager()
{
    // Components will be automatically destroyed by unique_ptr
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
    m_routeComparisonPanel->Initialize();
    m_routeDetailsPanel->Initialize();
    m_aiToolsPanel->Initialize();
    m_settingsPanel->Initialize();

    return true;
}

// Setup ImGui style
void UIManager::SetupImGuiStyle()
{
    // Apply theme based on current setting
    if (m_darkTheme) {
        ImGui::StyleColorsDark();
    }
    else {
        ImGui::StyleColorsLight();
    }

    ImGuiStyle& style = ImGui::GetStyle();

    // Adjust colors
    auto& colors = style.Colors;
    if (m_darkTheme) {
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
    }

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

    // Apply font scale
    io.FontGlobalScale = m_fontScale;
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
            if (ImGui::MenuItem("New Simulation", "Ctrl+N")) {
                // Reset simulation - regenerate waste levels
                m_application->RegenerateWasteLevels();
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Export Report", "Ctrl+E")) {
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
            ImGui::Separator();

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

            ImGui::Separator();

            if (ImGui::MenuItem("Recalculate Current Route", "F5")) {
                m_application->RecalculateCurrentRoute();
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("AI Tools")) {
            if (ImGui::MenuItem("Predict Waste Levels", "Ctrl+P")) {
                m_showAIWindow = true;
            }

            if (ImGui::MenuItem("Apply AI Route Optimization", "Ctrl+O")) {
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

    // 获取当前路线和废物位置
    const auto& wasteLocations = m_application->GetWasteLocations();
    const Route* currentRoute = m_application->GetCurrentRoute();

    // 使用RouteDetailsPanel组件渲染详情
    m_routeDetailsPanel->Render(wasteLocations, currentRoute);

    ImGui::End();
}

// Render the route comparison window
void UIManager::RenderComparisonWindow()
{
    ImGui::Begin("Route Comparison", &m_showComparisonWindow);

    // 使用RouteComparisonPanel组件渲染比较视图
    // 获取所有路线对象放入向量
    std::vector<Route*> routes;

    // 这里应该从application获取所有路线，而不仅仅是当前路线
    // 这个功能需要在Application中增加GetAllRoutes方法
    Route* currentRoute = m_application->GetCurrentRoute();
    if (currentRoute) {
        routes.push_back(currentRoute);
    }

    m_routeComparisonPanel->Render(routes);

    ImGui::End();
}

// Render the AI tools window
void UIManager::RenderAIWindow()
{
    ImGui::Begin("AI Tools", &m_showAIWindow);

    // 使用AIToolsPanel组件渲染AI工具
    m_aiToolsPanel->Render(
        m_application->GetWasteLevelPredictor(),
        m_application->GetRouteLearningAgent(),
        m_application->GetLocationClustering()
    );

    ImGui::End();
}

// Render the settings window
void UIManager::RenderSettingsWindow()
{
    ImGui::Begin("Settings", &m_showSettingsWindow);

    // 使用SettingsPanel组件渲染设置
    m_settingsPanel->Render(m_darkTheme);

    ImGui::End();
}

// Render status bar
void UIManager::RenderStatusBar()
{
    const float statusBarHeight = 25.0f;

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
    ImGui::SameLine(200);

    // Current time
    time_t now = time(nullptr);
    tm* timeinfo = localtime(&now);
    char timeStr[64];
    strftime(timeStr, sizeof(timeStr), "%H:%M:%S", timeinfo);

    ImGui::Text("Time: %s", timeStr);
    ImGui::SameLine(350);

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

// Toggle dark/light theme
void UIManager::ToggleTheme()
{
    m_darkTheme = !m_darkTheme;
    SetupImGuiStyle();
}

// Save UI configuration to file
void UIManager::SaveConfiguration(const std::string& filename)
{
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for saving configuration: " << filename << std::endl;
        return;
    }

    // Write UI settings
    file << "darkTheme=" << (m_darkTheme ? "true" : "false") << std::endl;
    file << "fontScale=" << m_fontScale << std::endl;
    file << "showMapWindow=" << (m_showMapWindow ? "true" : "false") << std::endl;
    file << "showDetailsWindow=" << (m_showDetailsWindow ? "true" : "false") << std::endl;
    file << "showComparisonWindow=" << (m_showComparisonWindow ? "true" : "false") << std::endl;
    file << "showAIWindow=" << (m_showAIWindow ? "true" : "false") << std::endl;
    file << "showSettingsWindow=" << (m_showSettingsWindow ? "true" : "false") << std::endl;

    file.close();
}

// Load UI configuration from file
void UIManager::LoadConfiguration(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for loading configuration: " << filename << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string key, value;

        if (std::getline(iss, key, '=') && std::getline(iss, value)) {
            // Parse UI settings
            if (key == "darkTheme") {
                m_darkTheme = (value == "true");
            }
            else if (key == "fontScale") {
                m_fontScale = std::stof(value);
            }
            else if (key == "showMapWindow") {
                m_showMapWindow = (value == "true");
            }
            else if (key == "showDetailsWindow") {
                m_showDetailsWindow = (value == "true");
            }
            else if (key == "showComparisonWindow") {
                m_showComparisonWindow = (value == "true");
            }
            else if (key == "showAIWindow") {
                m_showAIWindow = (value == "true");
            }
            else if (key == "showSettingsWindow") {
                m_showSettingsWindow = (value == "true");
            }
        }
    }

    file.close();

    // Apply loaded settings
    SetupImGuiStyle();
}