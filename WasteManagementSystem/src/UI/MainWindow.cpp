// MainWindow.cpp
// Implementation of the main window UI component
#include "MainWindow.h"
#include "imgui.h"

void MainWindow::Render() {
    // This is a placeholder for the main window implementation
    // The actual content is handled by UIManager's main rendering code
    // This class could be used for additional functionality in the future

    // Example status information that could be shown in the main window
    ImGui::Text("Waste Management System");
    ImGui::Separator();
    ImGui::Text("Status: Running");

    // System information
    ImGui::Text("System Info:");
    ImGui::BulletText("FPS: %.1f", ImGui::GetIO().Framerate);
    ImGui::BulletText("Frame Time: %.3f ms", 1000.0f / ImGui::GetIO().Framerate);

    // We could add more functionality here in the future
}