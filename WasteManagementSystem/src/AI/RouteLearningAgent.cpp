// RouteLearningAgent.cpp
// Implementation of the reinforcement learning agent for route optimization
#include "RouteLearningAgent.h"
#include <algorithm>
#include <numeric>
#include <iostream>
#include <fstream>
#include <sstream>
#include <random>
#include <cmath>

RouteLearningAgent::RouteLearningAgent()
    : m_learningRate(0.1f),
    m_discountFactor(0.9f),
    m_explorationRate(0.3f),
    m_numIterations(0)
{
    // Initialize random number generator
    std::random_device rd;
    m_rng = std::mt19937(rd());
}

RouteLearningAgent::~RouteLearningAgent()
{
}

void RouteLearningAgent::Initialize()
{
    // Initialize Q-table with zeros
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            // Skip if no direct path
            if (WasteLocation::map_distance_matrix[i][j] >= INF) {
                continue;
            }

            // Create a state with only location i visited
            std::vector<int> visited = { i };
            std::string state = EncodeState(visited, i);

            // Initialize Q-value for action j
            m_qTable[state][j] = 0.0f;
        }
    }

    // Clear experience buffer
    m_experienceBuffer.clear();
}

std::string RouteLearningAgent::EncodeState(const std::vector<int>& visitedLocations, int currentLocation)
{
    // Sort visited locations for consistent encoding
    std::vector<int> sortedVisited = visitedLocations;
    std::sort(sortedVisited.begin(), sortedVisited.end());

    // Build state string as "current:visited[0],visited[1],..."
    std::ostringstream oss;
    oss << currentLocation << ":";

    for (size_t i = 0; i < sortedVisited.size(); i++) {
        oss << sortedVisited[i];
        if (i < sortedVisited.size() - 1) {
            oss << ",";
        }
    }

    return oss.str();
}

std::vector<int> RouteLearningAgent::GetAvailableActions(const std::string& state, const std::vector<WasteLocation>& locations)
{
    // Parse the state string to get current location and visited locations
    std::istringstream iss(state);
    std::string currentStr;
    std::string visitedStr;

    std::getline(iss, currentStr, ':');
    std::getline(iss, visitedStr);

    int currentLocation = std::stoi(currentStr);

    // Parse visited locations
    std::vector<int> visitedLocations;
    std::istringstream visitedStream(visitedStr);
    std::string token;

    while (std::getline(visitedStream, token, ',')) {
        if (!token.empty()) {
            visitedLocations.push_back(std::stoi(token));
        }
    }

    // Get all location IDs with waste level >= 40%
    std::vector<int> validLocations;
    for (const auto& location : locations) {
        if (location.GetWasteLevel() >= 40.0f) {
            int id = WasteLocation::dict_Name_toId[location.GetLocationName()];
            if (id > 0) { // Skip station (0)
                validLocations.push_back(id);
            }
        }
    }

    // Add the station as a valid destination
    validLocations.push_back(0);

    // Filter out locations that have already been visited
    std::vector<int> availableActions;
    for (int location : validLocations) {
        // Skip if location has already been visited
        if (std::find(visitedLocations.begin(), visitedLocations.end(), location) != visitedLocations.end()) {
            continue;
        }

        // Skip if no direct path from current location
        if (WasteLocation::map_distance_matrix[currentLocation][location] >= INF) {
            continue;
        }

        availableActions.push_back(location);
    }

    return availableActions;
}

int RouteLearningAgent::ChooseAction(const std::string& state, const std::vector<int>& availableActions)
{
    // If no actions available, return -1
    if (availableActions.empty()) {
        return -1;
    }

    // Epsilon-greedy policy: explore with probability m_explorationRate
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    if (dist(m_rng) < m_explorationRate) {
        // Explore: choose a random action
        std::uniform_int_distribution<size_t> actionDist(0, availableActions.size() - 1);
        return availableActions[actionDist(m_rng)];
    }
    else {
        // Exploit: choose the action with the highest Q-value
        int bestAction = availableActions[0];
        float bestValue = -std::numeric_limits<float>::max();

        for (int action : availableActions) {
            // If state-action pair doesn't exist in Q-table, initialize it
            if (m_qTable.find(state) == m_qTable.end() ||
                m_qTable[state].find(action) == m_qTable[state].end()) {
                m_qTable[state][action] = 0.0f;
            }

            float qValue = m_qTable[state][action];
            if (qValue > bestValue) {
                bestValue = qValue;
                bestAction = action;
            }
        }

        return bestAction;
    }
}

float RouteLearningAgent::CalculateReward(int fromLocation, int toLocation, float wasteLevel)
{
    // Calculate distance-based penalty
    float distance = WasteLocation::map_distance_matrix[fromLocation][toLocation];
    if (distance >= INF) {
        distance = 1000.0f; // Large but finite value for unreachable locations
    }

    float distancePenalty = -distance; // Negative reward proportional to distance

    // Calculate waste level reward (higher is better)
    float wasteLevelReward = 0.0f;
    if (toLocation > 0) { // Skip station (0)
        wasteLevelReward = wasteLevel / 10.0f; // Scale waste level reward
    }

    // Bonus for returning to station
    float returnBonus = 0.0f;
    if (toLocation == 0) {
        returnBonus = 5.0f;
    }

    // Combine rewards
    return distancePenalty + wasteLevelReward + returnBonus;
}

void RouteLearningAgent::UpdateQValue(const std::string& state, int action, float reward, const std::string& nextState)
{
    // If state-action pair doesn't exist in Q-table, initialize it
    if (m_qTable.find(state) == m_qTable.end() ||
        m_qTable[state].find(action) == m_qTable[state].end()) {
        m_qTable[state][action] = 0.0f;
    }

    // Get current Q-value
    float currentQ = m_qTable[state][action];

    // Calculate maximum Q-value for next state
    float maxNextQ = 0.0f;
    if (!nextState.empty() && m_qTable.find(nextState) != m_qTable.end()) {
        for (const auto& pair : m_qTable[nextState]) {
            maxNextQ = std::max(maxNextQ, pair.second);
        }
    }

    // Update Q-value using Q-learning update rule
    float newQ = currentQ + m_learningRate * (reward + m_discountFactor * maxNextQ - currentQ);

    // Store updated Q-value
    m_qTable[state][action] = newQ;
}

void RouteLearningAgent::TrainOnBatch()
{
    // If experience buffer is too small, do nothing
    if (m_experienceBuffer.size() < m_batchSize) {
        return;
    }

    // Select random batch from experience buffer
    std::uniform_int_distribution<size_t> dist(0, m_experienceBuffer.size() - 1);

    for (size_t i = 0; i < m_batchSize; i++) {
        size_t index = dist(m_rng);
        const Experience& exp = m_experienceBuffer[index];

        // Update Q-value for this experience
        UpdateQValue(exp.state, exp.action, exp.reward, exp.nextState);
    }
}

std::vector<int> RouteLearningAgent::GenerateEpisode(const std::vector<WasteLocation>& locations)
{
    // Start at the station
    int currentLocation = 0;
    std::vector<int> visitedLocations = { 0 };
    std::vector<int> route = { 0 };

    // Keep track of total reward
    float totalReward = 0.0f;

    // Limit episode length to prevent infinite loops
    const int maxSteps = 20;
    int steps = 0;

    while (steps < maxSteps) {
        steps++;

        // Encode current state
        std::string state = EncodeState(visitedLocations, currentLocation);

        // Get available actions
        std::vector<int> availableActions = GetAvailableActions(state, locations);

        // If no actions available or we're at the station and have visited at least one other location, end episode
        if (availableActions.empty() ||
            (currentLocation == 0 && visitedLocations.size() > 1 &&
                std::find(visitedLocations.begin(), visitedLocations.end(), 0) != visitedLocations.begin())) {
            break;
        }

        // Choose action
        int nextLocation = ChooseAction(state, availableActions);

        // If no valid action, end episode
        if (nextLocation == -1) {
            break;
        }

        // Get waste level of next location
        float wasteLevel = 0.0f;
        for (const auto& location : locations) {
            if (WasteLocation::dict_Name_toId[location.GetLocationName()] == nextLocation) {
                wasteLevel = location.GetWasteLevel();
                break;
            }
        }

        // Calculate reward
        float reward = CalculateReward(currentLocation, nextLocation, wasteLevel);
        totalReward += reward;

        // Move to next location
        currentLocation = nextLocation;
        visitedLocations.push_back(currentLocation);
        route.push_back(currentLocation);

        // Encode next state
        std::string nextState = EncodeState(visitedLocations, currentLocation);

        // Store experience in replay buffer
        Experience exp = {
            state,
            nextLocation,
            reward,
            nextState,
            false
        };

        // Add to experience buffer
        m_experienceBuffer.push_back(exp);

        // If buffer is full, remove oldest experiences
        if (m_experienceBuffer.size() > m_maxBufferSize) {
            m_experienceBuffer.erase(m_experienceBuffer.begin());
        }

        // Train on batch of experiences
        if (m_experienceBuffer.size() >= m_batchSize && steps % 5 == 0) {
            TrainOnBatch();
        }
    }

    // If route doesn't end at station, add it
    if (!route.empty() && route.back() != 0) {
        route.push_back(0);
    }

    return route;
}

void RouteLearningAgent::Train(const std::vector<WasteLocation>& locations, int iterations)
{
    std::cout << "Training route learning agent..." << std::endl;

    // Initialize exploration rate
    m_explorationRate = 0.3f;

    // Train for the specified number of iterations
    for (int i = 0; i < iterations; i++) {
        // Generate an episode
        std::vector<int> route = GenerateEpisode(locations);

        // Decay exploration rate
        m_explorationRate = std::max(0.05f, m_explorationRate * 0.99f);

        // Print progress
        if (i % 10 == 0) {
            std::cout << "Training iteration " << i << "/" << iterations << std::endl;
        }

        // Increment iteration counter
        m_numIterations++;
    }

    std::cout << "Training complete after " << m_numIterations << " iterations" << std::endl;
}

std::vector<int> RouteLearningAgent::GetBestRoute(const std::vector<WasteLocation>& locations)
{
    // Set exploration rate to 0 for best route
    float oldExplorationRate = m_explorationRate;
    m_explorationRate = 0.0f;

    // Generate an episode with no exploration
    std::vector<int> bestRoute = GenerateEpisode(locations);

    // Restore exploration rate
    m_explorationRate = oldExplorationRate;

    return bestRoute;
}

void RouteLearningAgent::Reset()
{
    // Clear Q-table and experience buffer
    m_qTable.clear();
    m_experienceBuffer.clear();

    // Reset iteration counter
    m_numIterations = 0;

    // Reinitialize
    Initialize();
}

float RouteLearningAgent::GetQValue(const std::string& state, int action)
{
    // If state-action pair doesn't exist in Q-table, return 0
    if (m_qTable.find(state) == m_qTable.end() ||
        m_qTable[state].find(action) == m_qTable[state].end()) {
        return 0.0f;
    }

    return m_qTable[state][action];
}

float** RouteLearningAgent::GetQMatrix()
{
    // Allocate memory for Q-matrix
    float** qMatrix = new float* [8];
    for (int i = 0; i < 8; i++) {
        qMatrix[i] = new float[8];

        // Initialize with zeros
        for (int j = 0; j < 8; j++) {
            qMatrix[i][j] = 0.0f;
        }
    }

    // For each state-action pair in the Q-table
    for (const auto& statePair : m_qTable) {
        // Parse the state string to get current location
        std::istringstream iss(statePair.first);
        std::string currentStr;
        std::getline(iss, currentStr, ':');

        int fromLocation = std::stoi(currentStr);

        // For each action
        for (const auto& actionPair : statePair.second) {
            int toLocation = actionPair.first;
            float qValue = actionPair.second;

            // Only update if locations are valid
            if (fromLocation >= 0 && fromLocation < 8 && toLocation >= 0 && toLocation < 8) {
                qMatrix[fromLocation][toLocation] = qValue;
            }
        }
    }

    return qMatrix;
}

std::pair<float, float> RouteLearningAgent::GetLearningMetrics()
{
    // For demonstration, return placeholder values
    return { 0.75f, 0.1f };
}

void RouteLearningAgent::SaveModel(const std::string& filename)
{
    // Open file for writing
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }

    // Write number of state-action pairs
    size_t numPairs = 0;
    for (const auto& statePair : m_qTable) {
        numPairs += statePair.second.size();
    }

    file << numPairs << std::endl;

    // Write each state-action pair and its Q-value
    for (const auto& statePair : m_qTable) {
        for (const auto& actionPair : statePair.second) {
            file << statePair.first << " " << actionPair.first << " " << actionPair.second << std::endl;
        }
    }

    file.close();

    std::cout << "Model saved to: " << filename << std::endl;
}

void RouteLearningAgent::LoadModel(const std::string& filename)
{
    // Open file for reading
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }

    // Clear existing Q-table
    m_qTable.clear();

    // Read number of state-action pairs
    size_t numPairs;
    file >> numPairs;

    // Read each state-action pair and its Q-value
    for (size_t i = 0; i < numPairs; i++) {
        std::string state;
        int action;
        float qValue;

        file >> state >> action >> qValue;

        m_qTable[state][action] = qValue;
    }

    file.close();

    std::cout << "Model loaded from: " << filename << std::endl;
}