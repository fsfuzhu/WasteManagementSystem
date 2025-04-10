// NonOptimizedRoute.cpp
// Implementation of the non-optimized route algorithm
#include "pch.h"
#include "NonOptimizedRoute.h"

NonOptimizedRoute::NonOptimizedRoute()
    : Route("Non-Optimized Route", 40.0f), // 40% waste threshold
    m_pickupRequired(false)
{
}

NonOptimizedRoute::~NonOptimizedRoute()
{
}

std::vector<int> NonOptimizedRoute::FilterDestinations(const std::vector<WasteLocation>& locations)
{
    std::vector<int> filteredDestinations;

    // Add locations that meet the waste threshold and are within distance limit
    for (const auto& location : locations) {
        if (location.GetWasteLevel() >= m_wasteThreshold) {
            int id = WasteLocation::dict_Name_toId[location.GetLocationName()];

            // 使用直线距离检查是否在最大距离范围内
            if (id > 0 && WasteLocation::map_distance_matrix[0][id] <= m_maxDistanceFromStation) {
                filteredDestinations.push_back(id);
            }
        }
    }

    return filteredDestinations;
}

std::vector<int> NonOptimizedRoute::GenerateDefaultRoute()
{
    // 对于非优化路线，我们将按点ID的顺序访问所有符合条件的点
    // 这里只返回一个基本框架，实际路线将在CalculateRoute中生成
    return { 0 }; // 只包含起始点，其余点将根据筛选结果添加
}

bool NonOptimizedRoute::CalculateRoute(const std::vector<WasteLocation>& locations)
{
    // Clear previous route data
    m_finalRoute.clear();
    m_individualDistances.clear();
    m_totalDistance = 0.0f;
    m_timeTaken = 0.0f;
    m_fuelConsumption = 0.0f;
    m_wage = 0.0f;
    m_totalCost = 0.0f;

    // Filter destinations based on waste level and distance
    m_filteredDestinations = FilterDestinations(locations);

    // Check if any locations need pickup
    if (m_filteredDestinations.empty()) {
        m_pickupRequired = false;
        return false;
    }

    m_pickupRequired = true;

    // 对于非优化路线，我们简单地按顺序访问所有符合条件的点
    m_finalRoute.push_back(0); // 从站点开始

    // 复制过滤后的目的地，并按ID排序
    std::vector<int> sortedDestinations = m_filteredDestinations;
    std::sort(sortedDestinations.begin(), sortedDestinations.end());

    // 按ID顺序添加所有点
    for (int id : sortedDestinations) {
        m_finalRoute.push_back(id);
    }

    // 返回站点
    m_finalRoute.push_back(0);

    // Calculate individual segment distances
    for (size_t i = 0; i < m_finalRoute.size() - 1; i++) {
        int fromId = m_finalRoute[i];
        int toId = m_finalRoute[i + 1];

        // 使用直线距离
        float distance = WasteLocation::map_distance_matrix[fromId][toId];
        m_individualDistances.push_back(distance);
    }

    // Calculate costs
    CalculateCosts();

    return true;
}

const std::vector<int>& NonOptimizedRoute::GetFilteredDestinations() const
{
    return m_filteredDestinations;
}

bool NonOptimizedRoute::IsPickupRequired() const
{
    return m_pickupRequired;
}