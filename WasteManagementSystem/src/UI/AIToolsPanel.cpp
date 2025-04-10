// AIToolsPanel.cpp
// Implementation of the AI tools panel UI component
#include "pch.h"
#include "UI/AIToolsPanel.h"
#include "UI/UIHelpers.h"
#include "Core/WasteLocation.h"

bool AIToolsPanel::Initialize()
{
    return true;
}

void AIToolsPanel::Render(WasteLevelPredictor* predictor, RouteLearningAgent* agent, LocationClustering* clustering)
{
    if (!predictor || !agent || !clustering) {
        ImGui::Text("AI components not initialized.");
        return;
    }

    // Create tabs for different AI tools
    if (ImGui::BeginTabBar("AIToolsTabBar")) {
        if (ImGui::BeginTabItem("Waste Level Prediction")) {
            RenderWastePredictionUI(predictor);
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Route Learning")) {
            RenderRouteLearningUI(agent);
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Location Clustering")) {
            RenderLocationClusteringUI(clustering);
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }
}

void AIToolsPanel::RenderWastePredictionUI(WasteLevelPredictor* predictor)
{
    // Header section
    UIHelpers::DrawSectionHeader("Waste Level Prediction");
    ImGui::Text("Predict future waste levels for all locations");
    ImGui::Spacing();

    // Settings for forecast
    ImGui::SliderInt("Forecast Days", &m_forecastDays, 1, 14, "%d days");

    // Prediction action button
    if (ImGui::Button("Generate Forecast", ImVec2(150, 30))) {
        // Generate predictions
        // Note: in a real implementation, we would store the predictions
        // and display them in the plot
    }

    ImGui::SameLine();
    UIHelpers::AddTooltip("Predict waste levels for all locations for the specified number of days");

    // Visualize the forecast if data is available
    ImGui::Spacing();
    ImGui::Separator();
    PlotForecast(predictor, m_forecastDays);

    // Display additional information
    ImGui::Spacing();
    UIHelpers::DrawSectionHeader("Collection Recommendations");
    ImGui::Text("Based on current waste levels and growth rates:");

    // In a real implementation, we would get this data from the predictor
    // For this example, we'll use placeholder data
    ImGui::Columns(3, "collection_recommendations");
    ImGui::Text("Location"); ImGui::NextColumn();
    ImGui::Text("Current Level (%)"); ImGui::NextColumn();
    ImGui::Text("Recommended Collection"); ImGui::NextColumn();
    ImGui::Separator();

    // Example data (in real implementation, get from predictor)
    const char* locations[] = { "A", "B", "C", "D", "E", "F", "G" };
    const float currentLevels[] = { 68.0f, 42.0f, 85.0f, 27.0f, 35.0f, 62.0f, 19.0f };
    const int recommendedDays[] = { 0, 2, 0, 5, 3, 0, 7 };

    for (int i = 0; i < 7; i++) {
        ImGui::Text("%s", locations[i]);
        ImGui::NextColumn();

        // Color based on level
        ImVec4 color;
        if (currentLevels[i] < 30.0f) {
            color = ImVec4(0.0f, 1.0f, 0.0f, 1.0f); // Green
        }
        else if (currentLevels[i] < 60.0f) {
            color = ImVec4(1.0f, 1.0f, 0.0f, 1.0f); // Yellow
        }
        else {
            color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f); // Red
        }

        ImGui::TextColored(color, "%.1f%%", currentLevels[i]);
        ImGui::NextColumn();

        // Recommendation
        if (recommendedDays[i] == 0) {
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Collect today");
        }
        else {
            ImGui::Text("In %d days", recommendedDays[i]);
        }
        ImGui::NextColumn();
    }

    ImGui::Columns(1);
}

void AIToolsPanel::RenderRouteLearningUI(RouteLearningAgent* agent)
{
    // Header section
    UIHelpers::DrawSectionHeader("Route Learning");
    ImGui::Text("Train AI agent to find optimal collection routes");
    ImGui::Spacing();

    // Training settings
    ImGui::SliderInt("Training Iterations", &m_trainingIterations, 10, 1000);

    // Training button
    if (ImGui::Button("Train Agent", ImVec2(120, 30))) {
        // Train the agent
        // In a real implementation, we would call agent->Train(locations, m_trainingIterations)
    }

    ImGui::SameLine();
    if (ImGui::Button("Reset Agent", ImVec2(120, 30))) {
        // Reset the agent
        // In a real implementation, we would call agent->Reset()
    }

    // Learning progress
    ImGui::Spacing();
    UIHelpers::DrawSectionHeader("Learning Progress");

    // Learning metrics
    std::pair<float, float> metrics = agent->GetLearningMetrics();
    ImGui::Text("Average Reward: %.2f", metrics.first);
    ImGui::Text("Training Loss: %.4f", metrics.second);

    // Learning curve
    ImGui::Spacing();
    PlotLearningCurve(agent);

    // Q-value matrix
    ImGui::Spacing();
    UIHelpers::DrawSectionHeader("Q-Value Matrix");
    RenderQValueMatrix(agent);
}

void AIToolsPanel::RenderLocationClusteringUI(LocationClustering* clustering)
{
    // Header section
    UIHelpers::DrawSectionHeader("Location Clustering");
    ImGui::Text("Group waste locations for efficient collection routes");
    ImGui::Spacing();

    // Clustering settings
    ImGui::SliderInt("Number of Clusters", &m_numClusters, 2, 5);

    // Weight sliders
    static float distanceWeight = 0.7f;
    static float wasteLevelWeight = 0.3f;

    ImGui::SliderFloat("Distance Weight", &distanceWeight, 0.1f, 0.9f, "%.1f");
    ImGui::SliderFloat("Waste Level Weight", &wasteLevelWeight, 0.1f, 0.9f, "%.1f");

    // Update weights in clustering component
    clustering->SetDistanceWeight(distanceWeight);
    clustering->SetWasteLevelWeight(wasteLevelWeight);

    // Run clustering button
    if (ImGui::Button("Run Clustering", ImVec2(150, 30))) {
        // In a real implementation, we would get locations and run clustering
        // clustering->RunClustering(locations, 100);
        clustering->SetNumClusters(m_numClusters);
    }

    // Cluster visualization
    ImGui::Spacing();
    UIHelpers::DrawSectionHeader("Cluster Visualization");

    // This would require the location data, which we don't have direct access to here
    // In a real implementation, we'd need to pass the locations to this method
    // For now, we'll use a placeholder message
    ImGui::Text("Cluster visualization would appear here.");

    // Cluster statistics
    ImGui::Spacing();
    UIHelpers::DrawSectionHeader("Cluster Statistics");

    ImGui::Columns(4, "cluster_stats");
    ImGui::Text("Cluster"); ImGui::NextColumn();
    ImGui::Text("Locations"); ImGui::NextColumn();
    ImGui::Text("Avg. Waste Level"); ImGui::NextColumn();
    ImGui::Text("Total Distance"); ImGui::NextColumn();
    ImGui::Separator();

    // Example data (in real implementation, get from clustering)
    for (int i = 0; i < m_numClusters; i++) {
        ImGui::Text("Cluster %d", i + 1);
        ImGui::NextColumn();

        // Locations (placeholder)
        ImGui::Text("A, C, F");
        ImGui::NextColumn();

        // Average waste level (placeholder)
        ImGui::Text("62.5%%");
        ImGui::NextColumn();

        // Total distance (placeholder)
        ImGui::Text("15.3 km");
        ImGui::NextColumn();
    }

    ImGui::Columns(1);
}

void AIToolsPanel::PlotForecast(WasteLevelPredictor* predictor, int days)
{
    // Generate forecast data
    // In a real implementation, we would call predictor->GenerateForecasts(days)

    // For this example, we'll use placeholder data
    const int numLocations = 7;
    const char* locationNames[] = { "A", "B", "C", "D", "E", "F", "G" };

    // Create a line for each location
    if (ImPlot::BeginPlot("Waste Level Forecast", ImVec2(-1, 300))) {
        ImPlot::SetupAxes("Days", "Waste Level (%)", ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_AutoFit);
        ImPlot::SetupAxisLimits(ImAxis_X1, 0, days, ImGuiCond_Always);
        ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 100, ImGuiCond_Always);

        // X-axis values (days)
        float x[15];
        for (int i = 0; i <= days; i++) {
            x[i] = i;
        }

        // Create random forecast data
        // In a real implementation, this would come from the predictor
        static float y[7][15]; // 7 locations, up to 14 days

        // Initialize with some random values if first time
        static bool initialized = false;
        if (!initialized) {
            for (int i = 0; i < numLocations; i++) {
                float baseLevel = 30.0f + static_cast<float>(rand() % 40);
                float growth = 5.0f + static_cast<float>(rand() % 10) / 10.0f;

                y[i][0] = baseLevel;
                for (int j = 1; j <= 14; j++) {
                    y[i][j] = std::min(100.0f, y[i][j - 1] + growth);
                }
            }
            initialized = true;
        }

        // Plot each location's forecast
        for (int i = 0; i < numLocations; i++) {
            ImPlot::PlotLine(locationNames[i], x, y[i], days + 1);
        }

        // Add a horizontal line at the 60% threshold
        static float threshold[2] = { 60.0f, 60.0f };
        static float thresholdX[2] = { 0.0f, 14.0f };
        ImPlot::PushStyleColor(ImPlotCol_Line, ImVec4(1.0f, 0.0f, 0.0f, 0.5f));
        ImPlot::PlotLine("Threshold", thresholdX, threshold, 2);
        ImPlot::PopStyleColor();

        ImPlot::EndPlot();
    }
}

void AIToolsPanel::PlotLearningCurve(RouteLearningAgent* agent)
{
    // This would display the learning progress over iterations
    // For this example, we'll use placeholder data

    if (ImPlot::BeginPlot("Learning Curve", ImVec2(-1, 200))) {
        ImPlot::SetupAxes("Iterations", "Average Reward", ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_AutoFit);

        // Generate placeholder data
        static const int numPoints = 100;
        static float x[numPoints];
        static float y[numPoints];
        static bool initialized = false;

        if (!initialized) {
            for (int i = 0; i < numPoints; i++) {
                x[i] = i;
                y[i] = -20.0f + 19.0f * (1.0f - std::exp(-0.05f * i));
                // Add some noise
                y[i] += (rand() % 100) / 100.0f - 0.5f;
            }
            initialized = true;
        }

        ImPlot::PlotLine("Reward", x, y, numPoints);
        ImPlot::EndPlot();
    }
}

void AIToolsPanel::RenderQValueMatrix(RouteLearningAgent* agent)
{
    // This would display the Q-value matrix in a heatmap
    // For a simple implementation, we'll use a table

    ImGui::Text("Q-values represent the expected future reward for taking each action from each state.");
    ImGui::Spacing();

    if (ImGui::BeginTable("QValueMatrix", 8, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
        // Header row with destination locations
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Text("From \\ To");

        for (int i = 0; i < 8; i++) {
            ImGui::TableNextColumn();
            ImGui::Text("%s", WasteLocation::dict_Id_to_Name[i].c_str());
        }

        // Get Q-value matrix from agent
        float** qMatrix = agent->GetQMatrix();

        // Rows for each source location
        for (int i = 0; i < 8; i++) {
            ImGui::TableNextRow();

            // First column is the source location
            ImGui::TableNextColumn();
            ImGui::Text("%s", WasteLocation::dict_Id_to_Name[i].c_str());

            // Remaining columns are Q-values for each destination
            for (int j = 0; j < 8; j++) {
                ImGui::TableNextColumn();

                if (i == j) {
                    ImGui::Text("-");
                }
                else {
                    // Color based on Q-value: higher is better (green), lower is worse (red)
                    float normalizedQ = (qMatrix[i][j] + 20.0f) / 40.0f; // Normalize to [0,1]
                    normalizedQ = std::max(0.0f, std::min(1.0f, normalizedQ));

                    ImVec4 color(
                        1.0f - normalizedQ, // Red component
                        normalizedQ,        // Green component
                        0.0f,               // Blue component
                        1.0f                // Alpha
                    );

                    ImGui::TextColored(color, "%.2f", qMatrix[i][j]);
                }
            }
        }

        // Clean up the matrix
        for (int i = 0; i < 8; i++) {
            delete[] qMatrix[i];
        }
        delete[] qMatrix;

        ImGui::EndTable();
    }
}