// GreedyRoute.cpp
// Implementation of the Greedy route algorithm
#include "pch.h"
#include "GreedyRoute.h"

GreedyRoute::GreedyRoute()
    : Route("Greedy Route", 30.0f) // 30% waste threshold for greedy route
{
}

GreedyRoute::~GreedyRoute()
{
}

std::vector<int> GreedyRoute::FilterDestinationsByWasteLevel(const std::vector<WasteLocation>& locations)
{
    std::vector<int> filteredDestinations;

    // Add locations that meet the waste threshold, regardless of distance
    for (const auto& location : locations) {
        if (location.GetWasteLevel() >= m_wasteThreshold) {
            int id = WasteLocation::dict_Name_toId[location.GetLocationName()];
            if (id > 0) { // Skip station (0)
                filteredDestinations.push_back(id);
            }
        }
    }

    return filteredDestinations;
}
std::vector<int> GreedyRoute::PathReconstruction(int start, int end, const int matrix[8][8])
{
    std::vector<int> path;
    while (start != end)
    {
        path.push_back(start);
        start = matrix[start][end];
    }
    path.push_back(end);
    return path;
}
std::vector<int> GreedyRoute::ExpandRouteWithIntermediateNodes(const std::vector<int>& basicRoute)
{
    std::vector<int> expandedRoute;

    // ��ӵ�һ���ڵ�
    if (!basicRoute.empty()) {
        expandedRoute.push_back(basicRoute[0]);
    }

    // ��չ·���������ظ�
    for (size_t i = 0; i < basicRoute.size() - 1; i++) {
        int start = basicRoute[i];
        int end = basicRoute[i + 1];

        // �ѷ��ʵĽڵ� (�����ظ�)
        std::vector<int> visited = expandedRoute;

        // �ҵ���start��end�����·��
        std::vector<int> path = PathReconstruction(start, end, OptimizedRoute::s_shortestRouteMatrix);

        // ����м�ڵ� (������㣬���Ѿ���expandedRoute��)
        for (size_t j = 1; j < path.size(); j++) {
            // ֻ���δ���ʹ��Ľڵ㣬���˱�����ʵ��յ�
            if (std::find(visited.begin(), visited.end(), path[j]) == visited.end() || path[j] == end) {
                expandedRoute.push_back(path[j]);
                visited.push_back(path[j]);
            }
        }
    }

    return expandedRoute;
}
std::vector<int> GreedyRoute::GenerateGreedyRoute(const std::vector<int>& destinations)
{
    // Always start from the station
    std::vector<int> route;
    route.push_back(0); // Station

    // Copy destinations to a mutable list
    std::vector<int> remaining = destinations;

    // Current position is the station
    int current = 0;

    // Build the route by repeatedly finding the nearest unvisited location
    while (!remaining.empty()) {
        float minDistance = std::numeric_limits<float>::max();
        int nearestIndex = -1;

        // Find the nearest unvisited location
        for (size_t i = 0; i < remaining.size(); i++) {
            // ʹ��ֱ�߾���
            float distance = WasteLocation::map_distance_matrix[current][remaining[i]];

            if (distance < minDistance) {
                minDistance = distance;
                nearestIndex = i;
            }
        }

        // Move to the nearest location
        if (nearestIndex >= 0) {
            current = remaining[nearestIndex];
            route.push_back(current);

            // Remove the visited location
            remaining.erase(remaining.begin() + nearestIndex);
        }
        else {
            // No reachable locations left
            break;
        }
    }

    // Return to the station
    route.push_back(0);

    return route;
}

bool GreedyRoute::CalculateRoute(const std::vector<WasteLocation>& locations) {
    // ɸѡ��Ҫ���ʵ�Ŀ�ĵ�
    m_filteredDestinations = FilterDestinationsByWasteLevel(locations);

    // ����Ƿ���Ҫ�ռ�
    m_pickupRequired = !m_filteredDestinations.empty();

    if (!m_pickupRequired) {
        return false; // û����Ҫ�ռ��ĵ�
    }

    // ����̰��·��
    std::vector<int> basicRoute = GenerateGreedyRoute(m_filteredDestinations);

    // ��չ·���԰����м�ڵ� - ������һ��
    m_finalRoute = ExpandRouteWithIntermediateNodes(basicRoute);

    // ����ÿ�ξ���
    m_individualDistances = CalculateSegmentDistances(m_finalRoute);

    // �����ܾ��롢ʱ��ͳɱ�
    CalculateCosts();

    return true;
}
std::vector<float> GreedyRoute::CalculateSegmentDistances(const std::vector<int>& route)
{
    std::vector<float> distances;

    // ����·����ÿ�����ڽڵ��֮��ľ���
    for (size_t i = 0; i < route.size() - 1; i++) {
        int from = route[i];
        int to = route[i + 1];
        float distance = WasteLocation::map_distance_matrix[from][to];
        distances.push_back(distance);
    }

    return distances;
}
const std::vector<int>& GreedyRoute::GetFilteredDestinations() const
{
    return m_filteredDestinations;
}

bool GreedyRoute::IsPickupRequired() const
{
    return m_pickupRequired;
}