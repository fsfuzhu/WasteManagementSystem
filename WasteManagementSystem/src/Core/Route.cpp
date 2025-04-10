// Route.cpp
// Implementation of the base Route class
#include "pch.h"
#include "Route.h"
#include "../Application.h"

Route::Route(const std::string& name, float threshold)
    : m_finalRoute(),
    m_individualDistances(),
    m_totalDistance(0.0f),
    m_timeTaken(0.0f),
    m_fuelConsumption(0.0f),
    m_wage(0.0f),
    m_totalCost(0.0f),
    m_routeName(name),
    m_wasteThreshold(threshold),
    m_app(nullptr)  // 初始化为nullptr
{
}

Route::~Route()
{
}

void Route::SetApplication(Application* app)
{
    m_app = app;
}

void Route::CalculateCosts()
{
    // 计算总距离
    m_totalDistance = 0.0f;
    for (float distance : m_individualDistances) {
        m_totalDistance += distance;
    }

    // 如果有应用程序指针，使用设置的值进行计算
    if (m_app) {
        m_timeTaken = ceil(m_app->GetDrivingSpeedMinPerKm() * m_totalDistance * 100.0f) / 100.0f;
        m_fuelConsumption = ceil(m_app->GetFuelCostPerKm() * m_totalDistance * 100.0f) / 100.0f;
        m_wage = ceil(m_app->GetDriverWagePerHour() * (m_timeTaken / 60.0f) * 100.0f) / 100.0f;
    }
    else {
        // 使用默认值
        m_timeTaken = ceil(1.5f * m_totalDistance * 100.0f) / 100.0f;
        m_fuelConsumption = ceil(1.5f * m_totalDistance * 100.0f) / 100.0f;
        m_wage = ceil(5.77f * (m_timeTaken / 60.0f) * 100.0f) / 100.0f;
    }

    m_totalCost = m_fuelConsumption + m_wage;
}

const std::vector<int>& Route::GetFinalRoute() const
{
    return m_finalRoute;
}

const std::vector<float>& Route::GetIndividualDistances() const
{
    return m_individualDistances;
}

float Route::GetTotalDistance() const
{
    return m_totalDistance;
}

float Route::GetTimeTaken() const
{
    return m_timeTaken;
}

float Route::GetFuelConsumption() const
{
    return m_fuelConsumption;
}

float Route::GetWage() const
{
    return m_wage;
}

float Route::GetTotalCost() const
{
    return m_totalCost;
}

std::string Route::GetRouteName() const
{
    return m_routeName;
}

float Route::GetWasteThreshold() const
{
    return m_wasteThreshold;
}