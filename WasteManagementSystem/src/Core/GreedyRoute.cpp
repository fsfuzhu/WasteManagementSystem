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

    // 添加第一个节点
    if (!basicRoute.empty()) {
        expandedRoute.push_back(basicRoute[0]);
    }

    // 扩展路径，避免重复
    for (size_t i = 0; i < basicRoute.size() - 1; i++) {
        int start = basicRoute[i];
        int end = basicRoute[i + 1];

        // 已访问的节点 (避免重复)
        std::vector<int> visited = expandedRoute;

        // 找到从start到end的最短路径
        std::vector<int> path = PathReconstruction(start, end, OptimizedRoute::s_shortestRouteMatrix);

        // 添加中间节点 (跳过起点，它已经在expandedRoute中)
        for (size_t j = 1; j < path.size(); j++) {
            // 只添加未访问过的节点，除了必须访问的终点
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
            // 使用直线距离
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
    // 筛选需要访问的目的地
    m_filteredDestinations = FilterDestinationsByWasteLevel(locations);

    // 检查是否需要收集
    m_pickupRequired = !m_filteredDestinations.empty();

    if (!m_pickupRequired) {
        return false; // 没有需要收集的点
    }

    // 生成贪婪路径
    std::vector<int> basicRoute = GenerateGreedyRoute(m_filteredDestinations);

    // 扩展路径以包含中间节点 - 新增这一步
    m_finalRoute = ExpandRouteWithIntermediateNodes(basicRoute);

    // 计算每段距离
    m_individualDistances = CalculateSegmentDistances(m_finalRoute);

    // 计算总距离、时间和成本
    CalculateCosts();

    return true;
}
std::vector<float> GreedyRoute::CalculateSegmentDistances(const std::vector<int>& route)
{
    std::vector<float> distances;

    // 计算路径上每个相邻节点对之间的距离
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