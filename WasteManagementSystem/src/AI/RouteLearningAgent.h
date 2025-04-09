// RouteLearningAgent.h
// This file defines an AI agent that learns optimal routes using reinforcement learning
#pragma once

#include "../Core/WasteLocation.h"
#include <vector>
#include <unordered_map>
#include <string>
#include <random>

/**
 * @brief AI agent that learns optimal routes using reinforcement learning
 */
class RouteLearningAgent {
private:
    /* Private members in RouteLearningAgent class */

    // Q-learning parameters
    float m_learningRate;
    float m_discountFactor;
    float m_explorationRate;
    int m_numIterations;

    // Q-table: state -> action -> Q-value
    // State is represented as a string encoding of visited locations
    // Action is the next location to visit
    std::unordered_map<std::string, std::unordered_map<int, float>> m_qTable;

    // Experience replay buffer
    struct Experience {
        std::string state;
        int action;
        float reward;
        std::string nextState;
        bool isDone;
    };

    std::vector<Experience> m_experienceBuffer;
    const size_t m_maxBufferSize = 10000;
    const size_t m_batchSize = 32;

    // Random number generator
    std::mt19937 m_rng;

    // Helper functions

    // Encode a state (set of visited locations) as a string
    std::string EncodeState(const std::vector<int>& visitedLocations, int currentLocation);

    // Get available actions (unvisited locations) from a state
    std::vector<int> GetAvailableActions(const std::string& state, const std::vector<WasteLocation>& locations);

    // Choose an action using epsilon-greedy policy
    int ChooseAction(const std::string& state, const std::vector<int>& availableActions);

    // Calculate reward for taking an action
    float CalculateReward(int fromLocation, int toLocation, float wasteLevel);

    // Update Q-value for a state-action pair
    void UpdateQValue(const std::string& state, int action, float reward, const std::string& nextState);

    // Train on a batch of experiences from the replay buffer
    void TrainOnBatch();

    // Generate an episode (complete route) using current policy
    std::vector<int> GenerateEpisode(const std::vector<WasteLocation>& locations);

public:
    /**
     * @brief Constructor for RouteLearningAgent
     */
    RouteLearningAgent();

    /**
     * @brief Destructor for RouteLearningAgent
     */
    ~RouteLearningAgent();

    /**
     * @brief Initialize the agent
     */
    void Initialize();

    /**
     * @brief Train the agent on waste locations
     * @param locations Vector of waste locations
     * @param iterations Number of training iterations
     */
    void Train(const std::vector<WasteLocation>& locations, int iterations);

    /**
     * @brief Get the best route according to learned policy
     * @param locations Vector of waste locations
     * @return Vector of location IDs forming the best route
     */
    std::vector<int> GetBestRoute(const std::vector<WasteLocation>& locations);

    /**
     * @brief Reset the learning agent
     */
    void Reset();

    /**
     * @brief Get the Q-value for a state-action pair
     * @param state State string
     * @param action Action (location ID)
     * @return Q-value
     */
    float GetQValue(const std::string& state, int action);

    /**
     * @brief Get Q-values for all locations
     * @return 2D matrix of Q-values [from][to]
     */
    float** GetQMatrix();

    /**
     * @brief Get learning metrics
     * @return Pair of average reward and loss over last episode
     */
    std::pair<float, float> GetLearningMetrics();

    /**
     * @brief Save the learned model to a file
     * @param filename Filename to save to
     */
    void SaveModel(const std::string& filename);

    /**
     * @brief Load a learned model from a file
     * @param filename Filename to load from
     */
    void LoadModel(const std::string& filename);
};