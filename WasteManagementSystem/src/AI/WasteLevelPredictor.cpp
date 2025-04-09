// WasteLevelPredictor.cpp
// Implementation of the AI-based waste level prediction component
#include "pch.h"
#include "WasteLevelPredictor.h"

// Helper function to get day of week (0 = Sunday, 6 = Saturday)
int GetDayOfWeek(time_t time) {
    struct tm* timeinfo = localtime(&time);
    return timeinfo->tm_wday;
}

// Helper function to get day of month (1-31)
int GetDayOfMonth(time_t time) {
    struct tm* timeinfo = localtime(&time);
    return timeinfo->tm_mday;
}

// Sigmoid activation function
float Sigmoid(float x) {
    return 1.0f / (1.0f + std::exp(-x));
}

WasteLevelPredictor::WasteLevelPredictor()
{
}

WasteLevelPredictor::~WasteLevelPredictor()
{
}

void WasteLevelPredictor::Initialize()
{
    // Initialize data for each location
    for (const auto& pair : WasteLocation::dict_Name_toId) {
        if (pair.first != "Station") { // Skip the station
            LocationData data;
            data.name = pair.first;
            data.accumulationRate = 0.0f;
            m_locationsData[pair.first] = data;

            // Default regression values
            m_regressionCoefficients[pair.first] = { 0.0f, 0.0f };
        }
    }

    // Generate initial synthetic data for training the model
    GenerateSyntheticData();

    // Initialize neural network
    InitializeNeuralNetwork();
}

void WasteLevelPredictor::GenerateSyntheticData()
{
    // Random number generator
    std::random_device rd;
    std::mt19937 gen(rd());

    // Current time
    time_t now = time(nullptr);

    // For each location, generate a month of synthetic data
    for (auto& pair : m_locationsData) {
        auto& data = pair.second;

        // Different waste accumulation rates for different locations
        float baseRate;
        if (pair.first == "A" || pair.first == "B") {
            // Commercial areas - higher on weekdays
            baseRate = 8.0f; // 8% per day
        }
        else if (pair.first == "C" || pair.first == "D") {
            // Residential areas - higher on weekends
            baseRate = 5.0f; // 5% per day
        }
        else {
            // Mixed areas
            baseRate = 6.5f; // 6.5% per day
        }

        data.accumulationRate = baseRate;

        // Generate 30 days of historical data
        float currentLevel = 0.0f;
        for (int day = 30; day >= 0; day--) {
            time_t pastTime = now - day * 24 * 60 * 60; // days ago

            // Apply day of week factor
            int dayOfWeek = GetDayOfWeek(pastTime);
            float dayFactor;

            if (pair.first == "A" || pair.first == "B") {
                // Commercial - higher on weekdays, lower on weekends
                dayFactor = (dayOfWeek == 0 || dayOfWeek == 6) ? 0.7f : 1.2f;
            }
            else if (pair.first == "C" || pair.first == "D") {
                // Residential - higher on weekends, lower on weekdays
                dayFactor = (dayOfWeek == 0 || dayOfWeek == 6) ? 1.4f : 0.9f;
            }
            else {
                // Mixed - relatively stable
                dayFactor = (dayOfWeek == 0 || dayOfWeek == 6) ? 1.1f : 0.95f;
            }

            // Apply some randomness
            std::normal_distribution<float> distribution(0.0f, 2.0f);
            float randomFactor = 1.0f + distribution(gen) / 100.0f;

            // Increase level
            currentLevel += baseRate * dayFactor * randomFactor;

            // Ensure level is within bounds
            currentLevel = std::max(0.0f, std::min(100.0f, currentLevel));

            // Add to historical data
            data.historicalLevels.push_back({ pastTime, currentLevel });

            // Reset level after periodic collection (every 5-7 days depending on location)
            int collectionPeriod;
            if (pair.first == "A" || pair.first == "B") {
                collectionPeriod = 5;
            }
            else if (pair.first == "C" || pair.first == "D") {
                collectionPeriod = 7;
            }
            else {
                collectionPeriod = 6;
            }

            if (day % collectionPeriod == 0) {
                currentLevel = 0.0f;
            }
        }
    }

    // Calculate initial regression coefficients
    for (auto& pair : m_locationsData) {
        auto& data = pair.second;
        float slope, intercept;
        PerformLinearRegression(data, slope, intercept);
        m_regressionCoefficients[pair.first] = { slope, intercept };
    }
}

void WasteLevelPredictor::InitializeNeuralNetwork()
{
    // Random number generator for weights
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(-0.5f, 0.5f);

    // For each location, initialize neural network weights
    for (const auto& pair : m_locationsData) {
        NeuralNetworkWeights weights;

        // Initialize input weights
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 4; j++) {
                weights.inputWeights[i][j] = dist(gen);
            }
        }

        // Initialize hidden weights
        for (int i = 0; i < 4; i++) {
            weights.hiddenWeights[i][0] = dist(gen);
        }

        // Initialize biases
        for (int i = 0; i < 4; i++) {
            weights.inputBias[i] = dist(gen);
        }
        weights.outputBias = dist(gen);

        m_nnWeights[pair.first] = weights;
    }
}

float WasteLevelPredictor::RunNeuralNetwork(const NeuralNetworkWeights& weights,
    float dayOfWeek, float dayOfMonth, float previousLevel)
{
    // Normalize inputs to [0,1] range
    float inputs[3] = {
        dayOfWeek / 6.0f,      // day of week (0-6) -> (0-1)
        dayOfMonth / 31.0f,    // day of month (1-31) -> (0-1)
        previousLevel / 100.0f  // previous level (0-100) -> (0-1)
    };

    // Hidden layer activations
    float hidden[4] = { 0 };

    // Calculate hidden layer
    for (int j = 0; j < 4; j++) {
        hidden[j] = weights.inputBias[j];
        for (int i = 0; i < 3; i++) {
            hidden[j] += inputs[i] * weights.inputWeights[i][j];
        }
        hidden[j] = Sigmoid(hidden[j]);
    }

    // Calculate output layer
    float output = weights.outputBias;
    for (int i = 0; i < 4; i++) {
        output += hidden[i] * weights.hiddenWeights[i][0];
    }
    output = Sigmoid(output);

    // Scale output back to 0-100 range
    return output * 100.0f;
}

void WasteLevelPredictor::TrainNeuralNetwork(int epochs)
{
    // For this demo, we'll just simulate a trained network
    // In a real implementation, this would use backpropagation to train on historical data
    std::cout << "Training neural network..." << std::endl;

    // Simulate training time
    for (int i = 0; i < epochs; i++) {
        std::cout << "Epoch " << (i + 1) << " of " << epochs << std::endl;
        // Training code would go here
    }

    std::cout << "Training complete!" << std::endl;
}

void WasteLevelPredictor::PerformLinearRegression(const LocationData& data, float& slope, float& intercept)
{
    // Need at least 2 data points for regression
    if (data.historicalLevels.size() < 2) {
        slope = data.accumulationRate; // Default slope based on accumulation rate
        intercept = data.historicalLevels.empty() ? 0.0f : data.historicalLevels.back().second;
        return;
    }

    // Extract x (time in days) and y (waste level) values
    std::vector<float> x;
    std::vector<float> y;

    // Use the most recent time as reference (x=0)
    time_t refTime = data.historicalLevels.back().first;

    for (const auto& pair : data.historicalLevels) {
        // Convert time to days from reference
        float days = difftime(pair.first, refTime) / (24.0f * 60.0f * 60.0f);
        x.push_back(days);
        y.push_back(pair.second);
    }

    // Calculate means
    float x_mean = std::accumulate(x.begin(), x.end(), 0.0f) / x.size();
    float y_mean = std::accumulate(y.begin(), y.end(), 0.0f) / y.size();

    // Calculate slope
    float numerator = 0.0f;
    float denominator = 0.0f;

    for (size_t i = 0; i < x.size(); i++) {
        numerator += (x[i] - x_mean) * (y[i] - y_mean);
        denominator += (x[i] - x_mean) * (x[i] - x_mean);
    }

    if (std::abs(denominator) < 1e-6f) {
        slope = data.accumulationRate; // Default to accumulation rate if denominator is zero
    }
    else {
        slope = numerator / denominator;
    }

    // Calculate intercept (y = mx + b -> b = y - mx)
    intercept = y_mean - slope * x_mean;
}

float WasteLevelPredictor::PredictWasteLevel(const std::string& locationName, int daysAhead)
{
    // Check if location exists
    if (m_locationsData.find(locationName) == m_locationsData.end()) {
        return 0.0f;
    }

    // Get latest waste level
    const auto& data = m_locationsData[locationName];
    if (data.historicalLevels.empty()) {
        return 0.0f;
    }

    float latestLevel = data.historicalLevels.back().second;

    // Get regression coefficients
    float slope = m_regressionCoefficients[locationName].first;
    float intercept = m_regressionCoefficients[locationName].second;

    // Linear prediction: level = slope * days + intercept
    float linearPrediction = intercept + slope * daysAhead;

    // Get future time
    time_t futureTime = time(nullptr) + daysAhead * 24 * 60 * 60;
    int dayOfWeek = GetDayOfWeek(futureTime);
    int dayOfMonth = GetDayOfMonth(futureTime);

    // Neural network prediction
    float nnPrediction = RunNeuralNetwork(m_nnWeights[locationName],
        static_cast<float>(dayOfWeek),
        static_cast<float>(dayOfMonth),
        latestLevel);

    // Combine predictions (70% NN, 30% linear)
    float combinedPrediction = 0.7f * nnPrediction + 0.3f * linearPrediction;

    // Ensure prediction is within valid range [0, 100]
    return std::max(0.0f, std::min(100.0f, combinedPrediction));
}

void WasteLevelPredictor::UpdateModel(const std::vector<WasteLocation>& locations)
{
    // Current time
    time_t now = time(nullptr);

    // Update historical data with current waste levels
    for (const auto& location : locations) {
        std::string name = location.GetLocationName();

        if (m_locationsData.find(name) != m_locationsData.end()) {
            auto& data = m_locationsData[name];

            // Add current level to historical data
            data.historicalLevels.push_back({ now, location.GetWasteLevel() });

            // Keep only last 30 data points
            if (data.historicalLevels.size() > 30) {
                data.historicalLevels.erase(data.historicalLevels.begin());
            }

            // Update regression model
            float slope, intercept;
            PerformLinearRegression(data, slope, intercept);
            m_regressionCoefficients[name] = { slope, intercept };
        }
    }

    // We would also update neural network weights here in a real implementation
    // For demo purposes, we'll skip this step
}

int WasteLevelPredictor::GetRecommendedCollectionDay(const std::string& locationName, float threshold)
{
    // Check if location exists
    if (m_locationsData.find(locationName) == m_locationsData.end()) {
        return -1;
    }

    // Get current waste level
    const auto& data = m_locationsData[locationName];
    if (data.historicalLevels.empty()) {
        return -1;
    }

    float currentLevel = data.historicalLevels.back().second;

    // Already over threshold
    if (currentLevel >= threshold) {
        return 0;
    }

    // Predict future waste levels until threshold is reached
    for (int day = 1; day <= 14; day++) {
        float predictedLevel = PredictWasteLevel(locationName, day);
        if (predictedLevel >= threshold) {
            return day;
        }
    }

    // If no day within 14 days reaches threshold, return -1
    return -1;
}

std::unordered_map<std::string, std::vector<float>> WasteLevelPredictor::GenerateForecasts(int days)
{
    std::unordered_map<std::string, std::vector<float>> forecasts;

    // Generate forecasts for each location
    for (const auto& pair : m_locationsData) {
        std::string locationName = pair.first;
        std::vector<float> forecast;

        // Get current level
        float currentLevel = 0.0f;
        if (!pair.second.historicalLevels.empty()) {
            currentLevel = pair.second.historicalLevels.back().second;
        }

        // Add current level
        forecast.push_back(currentLevel);

        // Predict future levels
        for (int day = 1; day <= days; day++) {
            float predictedLevel = PredictWasteLevel(locationName, day);
            forecast.push_back(predictedLevel);
        }

        forecasts[locationName] = forecast;
    }

    return forecasts;
}