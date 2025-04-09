// LocationClustering.h
// This file defines an AI component for clustering waste locations
#pragma once

#include "../Core/WasteLocation.h"
#include <vector>
#include <unordered_map>
#include <string>

/**
 * @brief AI component for clustering waste locations
 *
 * This class implements a K-means clustering algorithm to group waste
 * locations based on proximity and waste levels, allowing for more
 * efficient collection routes.
 */
class LocationClustering {
private:
    /* Private members in LocationClustering class */

    // Number of clusters
    int m_numClusters;

    // Cluster assignments for each location
    std::unordered_map<std::string, int> m_clusterAssignments;

    // Cluster centroids (x, y coordinates)
    std::vector<std::pair<float, float>> m_centroids;

    // Distance weights for clustering
    float m_distanceWeight;
    float m_wasteLevelWeight;

    // Maximum iterations for K-means
    int m_maxIterations;

    // Helper functions

    // Initialized centroids using K-means++ algorithm
    void InitializeCentroids(const std::vector<WasteLocation>& locations, int k);

    // Assign locations to nearest centroid
    bool AssignClusters(const std::vector<WasteLocation>& locations);

    // Update centroid positions
    void UpdateCentroids(const std::vector<WasteLocation>& locations);

    // Calculate weighted distance between two locations
    float CalculateDistance(const WasteLocation& loc1, const WasteLocation& loc2);

    // Calculate distance between a location and a centroid
    float CalculateDistanceToCentroid(const WasteLocation& location,
        const std::pair<float, float>& centroid);

    // Get index of location in the vector
    int GetLocationIndex(const std::vector<WasteLocation>& locations,
        const std::string& name);

public:
    /**
     * @brief Constructor for LocationClustering
     * @param numClusters Number of clusters to create
     */
    LocationClustering(int numClusters = 3);

    /**
     * @brief Destructor for LocationClustering
     */
    ~LocationClustering();

    /**
     * @brief Initialize the clustering algorithm
     */
    void Initialize();

    /**
     * @brief Run K-means clustering on waste locations
     * @param locations Vector of waste locations
     * @param iterations Maximum number of iterations
     * @return True if clustering was successful
     */
    bool RunClustering(const std::vector<WasteLocation>& locations, int iterations = 100);

    /**
     * @brief Get cluster assignment for a location
     * @param locationName Name of the location
     * @return Cluster ID
     */
    int GetClusterForLocation(const std::string& locationName) const;

    /**
     * @brief Get locations in a cluster
     * @param clusterId Cluster ID
     * @param locations Vector of all waste locations
     * @return Vector of location names in the cluster
     */
    std::vector<std::string> GetLocationsInCluster(int clusterId,
        const std::vector<WasteLocation>& locations) const;

    /**
     * @brief Get all cluster assignments
     * @return Map of location names to cluster IDs
     */
    const std::unordered_map<std::string, int>& GetClusterAssignments() const;

    /**
     * @brief Get number of clusters
     * @return Number of clusters
     */
    int GetNumClusters() const;

    /**
     * @brief Set number of clusters
     * @param numClusters Number of clusters
     */
    void SetNumClusters(int numClusters);

    /**
     * @brief Set distance weight for clustering
     * @param weight Weight for distance (0-1)
     */
    void SetDistanceWeight(float weight);

    /**
     * @brief Set waste level weight for clustering
     * @param weight Weight for waste level (0-1)
     */
    void SetWasteLevelWeight(float weight);

    /**
     * @brief Get cluster centroid
     * @param clusterId Cluster ID
     * @return Pair of (x, y) coordinates
     */
    std::pair<float, float> GetClusterCentroid(int clusterId) const;

    /**
     * @brief Get all cluster centroids
     * @return Vector of (x, y) coordinates
     */
    const std::vector<std::pair<float, float>>& GetClusterCentroids() const;

    /**
     * @brief Calculate average waste level in a cluster
     * @param clusterId Cluster ID
     * @param locations Vector of all waste locations
     * @return Average waste level (%)
     */
    float CalculateAverageWasteLevel(int clusterId, const std::vector<WasteLocation>& locations) const;

    /**
     * @brief Calculate total distance within a cluster
     * @param clusterId Cluster ID
     * @param locations Vector of all waste locations
     * @return Total distance (km)
     */
    float CalculateTotalDistance(int clusterId, const std::vector<WasteLocation>& locations) const;
};