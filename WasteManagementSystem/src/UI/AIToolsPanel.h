// AIToolsPanel.h
// This file defines the AI tools panel UI component
#pragma once

#include "../AI/WasteLevelPredictor.h"
#include "../AI/RouteLearningAgent.h"
#include "../AI/LocationClustering.h"

/**
 * @brief Panel for AI tools and visualizations
 */
class AIToolsPanel {
public:
    AIToolsPanel() = default;
    ~AIToolsPanel() = default;

    /**
     * @brief Initialize the AI tools panel
     * @return True if initialization was successful
     */
    bool Initialize();

    /**
     * @brief Render the AI tools panel
     * @param predictor Waste level predictor instance
     * @param agent Route learning agent instance
     * @param clustering Location clustering instance
     */
    void Render(WasteLevelPredictor* predictor,
        RouteLearningAgent* agent,
        LocationClustering* clustering);

private:
    /**
     * @brief Render waste prediction UI tab
     * @param predictor Waste level predictor instance
     */
    void RenderWastePredictionUI(WasteLevelPredictor* predictor);

    /**
     * @brief Render route learning UI tab
     * @param agent Route learning agent instance
     */
    void RenderRouteLearningUI(RouteLearningAgent* agent);

    /**
     * @brief Render location clustering UI tab
     * @param clustering Location clustering instance
     */
    void RenderLocationClusteringUI(LocationClustering* clustering);

    // State variables for the panel
    int m_forecastDays = 7;
    int m_trainingIterations = 100;
    int m_numClusters = 3;

    // Plot forecast data
    void PlotForecast(WasteLevelPredictor* predictor, int days);

    // Plot learning curve
    void PlotLearningCurve(RouteLearningAgent* agent);

    // Render Q-value matrix
    void RenderQValueMatrix(RouteLearningAgent* agent);

    // Render cluster visualization
    void RenderClusterVisualization(LocationClustering* clustering,
        const std::vector<WasteLocation>& locations);
};