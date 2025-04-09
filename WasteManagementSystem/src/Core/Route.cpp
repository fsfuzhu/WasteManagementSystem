// Route.cpp
// Implementation of the base Route class
#include "pch.h"
#include "Route.h"

Route::Route(const std::string& name, float threshold)
    : m_finalRoute(),
    m_individualDistances(),
    m_totalDistance(0.0f),
    m_timeTaken(0.0f),
    m_fuelConsumption(0.0f),
    m_wage(0.0f),
    m_totalCost(0.0f),
    m_routeName(name),
    m_wasteThreshold(threshold)
{
}

Route::~Route()
{
}

void Route::CalculateCosts()
{
    // Calculate total distance
    m_totalDistance = 0.0f;
    for (float distance : m_individualDistances) {
        m_totalDistance += distance;
    }

    // Calculate time taken (1.5 minutes per km)
    m_timeTaken = 1.5f * m_totalDistance;

    // Calculate fuel consumption (RM 1.5 per km)
    m_fuelConsumption = 1.5f * m_totalDistance;

    // Calculate wage (RM 5.77 per hour)
    m_wage = 5.77f * (m_timeTaken / 60.0f);

    // Calculate total cost
    m_totalCost = m_fuelConsumption + m_wage;

    // Round to 2 decimal places
    m_timeTaken = std::ceil(m_timeTaken * 100.0f) / 100.0f;
    m_fuelConsumption = std::ceil(m_fuelConsumption * 100.0f) / 100.0f;
    m_wage = std::ceil(m_wage * 100.0f) / 100.0f;
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