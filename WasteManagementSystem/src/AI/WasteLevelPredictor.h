// WasteLevelPredictor.h
// This file defines the AI component for predicting future waste levels
#pragma once

#include "../Core/WasteLocation.h"
#include <vector>
#include <string>
#include <ctime>
#include <unordered_map>
#include <utility>
#include <functional>

/**
 * @brief AI component to predict future waste levels using machine learning
 */
class WasteLevelPredictor {
private:
    /* Private members in WasteLevelPredictor class */

    // Structure to hold historical data for each location
    struct LocationData {
        std::string name;
        std::vector<std::pair<time_t, float>> historicalLevels;
        float accumulationRate;  // Average waste accumulation per day
    };

    std::unordered_map<std::string, LocationData> m_locationsData;

    // Linear regression coefficients for each location (slope, intercept)
    std::unordered_map<std::string, std::pair<float, float>> m_regressionCoefficients;

    // Perform linear regression on historical data
    void PerformLinearRegression(const LocationData& data, float& slope, float& intercept);

    // Generate synthetic training data for initial model
    void GenerateSyntheticData();

    // Set of trained neural network weights for each location - simplified for demo
    struct NeuralNetworkWeights {
        float inputWeights[3][4];    // 3 inputs, 4 hidden neurons
        float hiddenWeights[4][1];   // 4 hidden neurons, 1 output
        float inputBias[4];          // Bias for hidden layer
        float outputBias;            // Bias for output layer
    };

    std::unordered_map<std::string, NeuralNetworkWeights> m_nnWeights;

    // Initialize neural network with random weights
    void InitializeNeuralNetwork();

    // Run neural network prediction
    float RunNeuralNetwork(const NeuralNetworkWeights& weights,
        float dayOfWeek, float dayOfMonth, float previousLevel);

public:
    WasteLevelPredictor();
    ~WasteLevelPredictor();

    /**
     * @brief Initialize the predictor with initial data
     */
    void Initialize();

    /**
     * @brief Predict waste level for a location at a future time
     * @param locationName Name of the location
     * @param daysAhead Number of days in the future to predict
     * @return Predicted waste level (0-100%)
     */
    float PredictWasteLevel(const std::string& locationName, int daysAhead);

    /**
     * @brief Update the prediction model with new data
     * @param locations Current waste locations with their levels
     */
    void UpdateModel(const std::vector<WasteLocation>& locations);

    /**
     * @brief Get recommended collection day for a location
     * @param locationName Name of the location
     * @param threshold Waste level threshold for collection
     * @return Number of days until collection is recommended
     */
    int GetRecommendedCollectionDay(const std::string& locationName, float threshold);

    /**
     * @brief Train the neural network model with historical data
     * @param epochs Number of training epochs
     */
    void TrainNeuralNetwork(int epochs);

    /**
     * @brief Generate waste level forecasts for all locations
     * @param days Number of days to forecast
     * @return Map of location names to vector of daily waste level predictions
     */
    std::unordered_map<std::string, std::vector<float>> GenerateForecasts(int days);
};