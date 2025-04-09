// LocationClustering.cpp
// Implementation of the location clustering AI component
#include "pch.h"
#include "LocationClustering.h"


LocationClustering::LocationClustering(int numClusters)
    : m_numClusters(numClusters),
    m_distanceWeight(0.7f),
    m_wasteLevelWeight(0.3f),
    m_maxIterations(100)
{
}

LocationClustering::~LocationClustering()
{
}

void LocationClustering::Initialize()
{
    // Clear previous clustering data
    m_clusterAssignments.clear();
    m_centroids.clear();
}

void LocationClustering::InitializeCentroids(const std::vector<WasteLocation>& locations, int k)
{
    m_centroids.clear();

    // Random number generator
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, locations.size() - 1);

    // Implement K-means++ initialization

    // Choose first centroid randomly
    int firstIdx = dist(gen);
    std::string firstName = locations[firstIdx].GetLocationName();
    int firstId = WasteLocation::dict_Name_toId[firstName];
    m_centroids.push_back({
        WasteLocation::location_coordinates[firstId][0],
        WasteLocation::location_coordinates[firstId][1]
        });

    // Choose remaining centroids using K-means++
    for (int i = 1; i < k; i++) {
        // Calculate squared distances from each point to nearest centroid
        std::vector<float> distances(locations.size(), std::numeric_limits<float>::max());
        float totalDistance = 0.0f;

        for (size_t j = 0; j < locations.size(); j++) {
            std::string name = locations[j].GetLocationName();
            int id = WasteLocation::dict_Name_toId[name];

            // Skip station
            if (id == 0) {
                continue;
            }

            // Find distance to nearest centroid
            for (const auto& centroid : m_centroids) {
                float dx = WasteLocation::location_coordinates[id][0] - centroid.first;
                float dy = WasteLocation::location_coordinates[id][1] - centroid.second;
                float distance = dx * dx + dy * dy; // Squared distance

                distances[j] = std::min(distances[j], distance);
            }

            totalDistance += distances[j];
        }

        // Choose next centroid with probability proportional to squared distance
        std::discrete_distribution<> weightedDist(distances.begin(), distances.end());
        int nextIdx = weightedDist(gen);
        std::string nextName = locations[nextIdx].GetLocationName();
        int nextId = WasteLocation::dict_Name_toId[nextName];

        m_centroids.push_back({
            WasteLocation::location_coordinates[nextId][0],
            WasteLocation::location_coordinates[nextId][1]
            });
    }
}

bool LocationClustering::AssignClusters(const std::vector<WasteLocation>& locations)
{
    bool changed = false;

    // For each location, assign to nearest centroid
    for (const auto& location : locations) {
        std::string name = location.GetLocationName();
        int id = WasteLocation::dict_Name_toId[name];

        // Skip station
        if (id == 0) {
            continue;
        }

        // Find nearest centroid
        int nearestCluster = 0;
        float minDistance = std::numeric_limits<float>::max();

        for (int i = 0; i < m_numClusters; i++) {
            float dx = WasteLocation::location_coordinates[id][0] - m_centroids[i].first;
            float dy = WasteLocation::location_coordinates[id][1] - m_centroids[i].second;
            float distance = std::sqrt(dx * dx + dy * dy);

            // Apply waste level weight
            float wasteLevel = location.GetWasteLevel();
            float weightedDistance = m_distanceWeight * distance - m_wasteLevelWeight * wasteLevel;

            if (weightedDistance < minDistance) {
                minDistance = weightedDistance;
                nearestCluster = i;
            }
        }

        // Check if cluster assignment changed
        if (m_clusterAssignments.find(name) == m_clusterAssignments.end() ||
            m_clusterAssignments[name] != nearestCluster) {
            m_clusterAssignments[name] = nearestCluster;
            changed = true;
        }
    }

    return changed;
}

void LocationClustering::UpdateCentroids(const std::vector<WasteLocation>& locations)
{
    // Reset centroids
    std::vector<std::pair<float, float>> newCentroids(m_numClusters, { 0.0f, 0.0f });
    std::vector<int> clusterSizes(m_numClusters, 0);

    // Sum up coordinates for each cluster
    for (const auto& location : locations) {
        std::string name = location.GetLocationName();

        // Skip station and locations without cluster assignment
        if (name == "Station" || m_clusterAssignments.find(name) == m_clusterAssignments.end()) {
            continue;
        }

        int clusterId = m_clusterAssignments[name];
        int locId = WasteLocation::dict_Name_toId[name];

        newCentroids[clusterId].first += WasteLocation::location_coordinates[locId][0];
        newCentroids[clusterId].second += WasteLocation::location_coordinates[locId][1];
        clusterSizes[clusterId]++;
    }

    // Calculate average coordinates (centroid)
    for (int i = 0; i < m_numClusters; i++) {
        if (clusterSizes[i] > 0) {
            newCentroids[i].first /= clusterSizes[i];
            newCentroids[i].second /= clusterSizes[i];
        }
        else {
            // If cluster is empty, keep old centroid
            newCentroids[i] = m_centroids[i];
        }
    }

    m_centroids = newCentroids;
}

float LocationClustering::CalculateDistance(const WasteLocation& loc1, const WasteLocation& loc2)
{
    int id1 = WasteLocation::dict_Name_toId[loc1.GetLocationName()];
    int id2 = WasteLocation::dict_Name_toId[loc2.GetLocationName()];

    float dx = WasteLocation::location_coordinates[id1][0] - WasteLocation::location_coordinates[id2][0];
    float dy = WasteLocation::location_coordinates[id1][1] - WasteLocation::location_coordinates[id2][1];

    return std::sqrt(dx * dx + dy * dy);
}

float LocationClustering::CalculateDistanceToCentroid(const WasteLocation& location,
    const std::pair<float, float>& centroid)
{
    int id = WasteLocation::dict_Name_toId[location.GetLocationName()];

    float dx = WasteLocation::location_coordinates[id][0] - centroid.first;
    float dy = WasteLocation::location_coordinates[id][1] - centroid.second;

    return std::sqrt(dx * dx + dy * dy);
}

int LocationClustering::GetLocationIndex(const std::vector<WasteLocation>& locations,
    const std::string& name)
{
    for (size_t i = 0; i < locations.size(); i++) {
        if (locations[i].GetLocationName() == name) {
            return i;
        }
    }

    return -1;
}

bool LocationClustering::RunClustering(const std::vector<WasteLocation>& locations, int iterations)
{
    // Initialize centroids
    InitializeCentroids(locations, m_numClusters);

    // Run K-means algorithm
    bool changed = true;
    int iter = 0;

    while (changed && iter < iterations) {
        // Assign clusters
        changed = AssignClusters(locations);

        // Update centroids
        UpdateCentroids(locations);

        iter++;
    }

    std::cout << "K-means clustering completed after " << iter << " iterations" << std::endl;

    // Print cluster assignments
    for (int i = 0; i < m_numClusters; i++) {
        std::cout << "Cluster " << i << ": ";
        for (const auto& pair : m_clusterAssignments) {
            if (pair.second == i) {
                std::cout << pair.first << " ";
            }
        }
        std::cout << std::endl;
    }

    return true;
}

int LocationClustering::GetClusterForLocation(const std::string& locationName) const
{
    // Return -1 if location not found
    if (m_clusterAssignments.find(locationName) == m_clusterAssignments.end()) {
        return -1;
    }

    return m_clusterAssignments.at(locationName);
}

std::vector<std::string> LocationClustering::GetLocationsInCluster(int clusterId,
    const std::vector<WasteLocation>& locations) const
{
    std::vector<std::string> result;

    for (const auto& pair : m_clusterAssignments) {
        if (pair.second == clusterId) {
            result.push_back(pair.first);
        }
    }

    return result;
}

const std::unordered_map<std::string, int>& LocationClustering::GetClusterAssignments() const
{
    return m_clusterAssignments;
}

int LocationClustering::GetNumClusters() const
{
    return m_numClusters;
}

void LocationClustering::SetNumClusters(int numClusters)
{
    m_numClusters = numClusters;
}

void LocationClustering::SetDistanceWeight(float weight)
{
    m_distanceWeight = weight;
}

void LocationClustering::SetWasteLevelWeight(float weight)
{
    m_wasteLevelWeight = weight;
}

std::pair<float, float> LocationClustering::GetClusterCentroid(int clusterId) const
{
    if (clusterId >= 0 && clusterId < m_numClusters) {
        return m_centroids[clusterId];
    }

    return { 0.0f, 0.0f };
}

const std::vector<std::pair<float, float>>& LocationClustering::GetClusterCentroids() const
{
    return m_centroids;
}

float LocationClustering::CalculateAverageWasteLevel(int clusterId, const std::vector<WasteLocation>& locations) const
{
    float totalWasteLevel = 0.0f;
    int count = 0;

    for (const auto& pair : m_clusterAssignments) {
        if (pair.second == clusterId) {
            // Find location in the vector
            for (const auto& location : locations) {
                if (location.GetLocationName() == pair.first) {
                    totalWasteLevel += location.GetWasteLevel();
                    count++;
                    break;
                }
            }
        }
    }

    if (count > 0) {
        return totalWasteLevel / count;
    }

    return 0.0f;
}

float LocationClustering::CalculateTotalDistance(int clusterId, const std::vector<WasteLocation>& locations) const
{
    float totalDistance = 0.0f;

    // Get locations in this cluster
    std::vector<std::string> clusterLocations;
    for (const auto& pair : m_clusterAssignments) {
        if (pair.second == clusterId) {
            clusterLocations.push_back(pair.first);
        }
    }

    // Calculate total distance between all pairs of locations
    for (size_t i = 0; i < clusterLocations.size(); i++) {
        for (size_t j = i + 1; j < clusterLocations.size(); j++) {
            int id1 = WasteLocation::dict_Name_toId[clusterLocations[i]];
            int id2 = WasteLocation::dict_Name_toId[clusterLocations[j]];

            float distance = WasteLocation::map_distance_matrix[id1][id2];
            if (distance < INF) {
                totalDistance += distance;
            }
        }
    }

    return totalDistance;
}