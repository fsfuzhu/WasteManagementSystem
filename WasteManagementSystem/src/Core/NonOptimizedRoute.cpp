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

            // ʹ��ֱ�߾������Ƿ��������뷶Χ��
            if (id > 0 && WasteLocation::map_distance_matrix[0][id] <= m_maxDistanceFromStation) {
                filteredDestinations.push_back(id);
            }
        }
    }

    return filteredDestinations;
}

std::vector<int> NonOptimizedRoute::GenerateDefaultRoute()
{
    // ���ڷ��Ż�·�ߣ����ǽ�����ID��˳��������з��������ĵ�
    // ����ֻ����һ��������ܣ�ʵ��·�߽���CalculateRoute������
    return { 0 }; // ֻ������ʼ�㣬����㽫����ɸѡ������
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

    // ���ڷ��Ż�·�ߣ����Ǽ򵥵ذ�˳��������з��������ĵ�
    m_finalRoute.push_back(0); // ��վ�㿪ʼ

    // ���ƹ��˺��Ŀ�ĵأ�����ID����
    std::vector<int> sortedDestinations = m_filteredDestinations;
    std::sort(sortedDestinations.begin(), sortedDestinations.end());

    // ��ID˳��������е�
    for (int id : sortedDestinations) {
        m_finalRoute.push_back(id);
    }

    // ����վ��
    m_finalRoute.push_back(0);

    // Calculate individual segment distances
    for (size_t i = 0; i < m_finalRoute.size() - 1; i++) {
        int fromId = m_finalRoute[i];
        int toId = m_finalRoute[i + 1];

        // ʹ��ֱ�߾���
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