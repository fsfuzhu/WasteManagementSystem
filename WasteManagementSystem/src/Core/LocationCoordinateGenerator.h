// LocationCoordinateGenerator.h
#pragma once

#include <random>
#include <cmath>
#include <vector>

/**
 * @brief Utility class for generating random location coordinates
 */
class LocationCoordinateGenerator {
public:
    /**
     * @brief Generate random coordinates for waste locations
     * @param numLocations Number of locations to generate (excluding station)
     * @param mapWidth Width of the map area
     * @param mapHeight Height of the map area
     * @param minDistance Minimum distance between any two locations
     * @param maxDistance Maximum distance between any two locations
     * @return 2D vector of coordinates [location_id][x,y]
     */
    static std::vector<std::pair<float, float>> GenerateRandomCoordinates(
        int numLocations,
        float mapWidth,
        float mapHeight,
        float minDistance,
        float maxDistance)
    {
        std::vector<std::pair<float, float>> coordinates;

        // Add station at the center
        float centerX = mapWidth / 2.0f;
        float centerY = mapHeight / 2.0f;
        coordinates.push_back(std::make_pair(centerX, centerY));

        // Random number generator
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> distX(50.0f, mapWidth - 50.0f);
        std::uniform_real_distribution<float> distY(50.0f, mapHeight - 50.0f);

        // Generate coordinates for other locations
        for (int i = 0; i < numLocations; i++) {
            bool validLocation = false;
            std::pair<float, float> newCoord;

            // Try to find a valid location
            int maxAttempts = 100;
            int attempts = 0;

            while (!validLocation && attempts < maxAttempts) {
                attempts++;

                // Generate random coordinates
                newCoord.first = distX(gen);
                newCoord.second = distY(gen);

                // Check distance from station
                float dx = newCoord.first - centerX;
                float dy = newCoord.second - centerY;
                float distToStation = std::sqrt(dx * dx + dy * dy);

                // Convert pixel distance to km for comparison (using same scale factor as in CalculateDirectDistance)
                float distToStationKm = distToStation / 50.0f;

                if (distToStationKm < minDistance || distToStationKm > maxDistance) {
                    continue; // Try again
                }

                // Check distance from other locations
                validLocation = true;
                for (size_t j = 1; j < coordinates.size(); j++) {
                    dx = newCoord.first - coordinates[j].first;
                    dy = newCoord.second - coordinates[j].second;
                    float dist = std::sqrt(dx * dx + dy * dy) / 50.0f; // Convert to km

                    if (dist < minDistance) {
                        validLocation = false;
                        break;
                    }
                }
            }

            if (validLocation) {
                coordinates.push_back(newCoord);
            }
            else {
                // If we couldn't find a valid location after max attempts,
                // relax the constraints a bit and try again
                i--; // Try again for this location
                minDistance *= 0.9f; // Reduce minimum distance requirement
            }
        }

        return coordinates;
    }

    /**
     * @brief Calculate direct distance between two points in km
     * @param x1 First point x coordinate
     * @param y1 First point y coordinate
     * @param x2 Second point x coordinate
     * @param y2 Second point y coordinate
     * @return Distance in km
     */
    static float CalculateDistance(float x1, float y1, float x2, float y2) {
        float dx = x2 - x1;
        float dy = y2 - y1;
        float pixelDistance = std::sqrt(dx * dx + dy * dy);

        // Convert to km using the same scale factor as in the main code
        float kmDistance = pixelDistance / 50.0f;

        // Round to 1 decimal place
        return std::round(kmDistance * 10.0f) / 10.0f;
    }
};