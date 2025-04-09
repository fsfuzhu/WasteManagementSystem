// FileIO.h
// Utility functions for file input/output operations
#pragma once

#include "../Core/Route.h"
#include "../Core/WasteLocation.h"
#include <string>
#include <vector>
#include <fstream>
#include <unordered_map>

/**
 * @brief File I/O utilities for the waste management system
 */
namespace FileIO {
    /**
     * @brief Save route information to a file
     * @param route Route to save
     * @param filename Output filename
     * @return True if successful, false otherwise
     */
    bool SaveRouteToFile(const Route* route, const std::string& filename);

    /**
     * @brief Save waste location information to a file
     * @param locations Vector of waste locations
     * @param filename Output filename
     * @return True if successful, false otherwise
     */
    bool SaveWasteLocationsToFile(const std::vector<WasteLocation>& locations, const std::string& filename);

    /**
     * @brief Load waste location information from a file
     * @param filename Input filename
     * @param[out] locations Vector to store loaded waste locations
     * @return True if successful, false otherwise
     */
    bool LoadWasteLocationsFromFile(const std::string& filename, std::vector<WasteLocation>& locations);

    /**
     * @brief Save simulation report to a file
     * @param routes Vector of routes to compare
     * @param locations Vector of waste locations
     * @param filename Output filename
     * @return True if successful, false otherwise
     */
    bool SaveSimulationReport(const std::vector<Route*>& routes,
        const std::vector<WasteLocation>& locations,
        const std::string& filename);

    /**
     * @brief Export route data as CSV for analysis
     * @param route Route to export
     * @param filename Output filename
     * @return True if successful, false otherwise
     */
    bool ExportRouteAsCSV(const Route* route, const std::string& filename);

    /**
     * @brief Export waste levels as CSV for analysis
     * @param locations Vector of waste locations
     * @param filename Output filename
     * @return True if successful, false otherwise
     */
    bool ExportWasteLevelsAsCSV(const std::vector<WasteLocation>& locations, const std::string& filename);

    /**
     * @brief Export AI prediction data as CSV
     * @param predictions Map of location names to predicted waste levels
     * @param filename Output filename
     * @return True if successful, false otherwise
     */
    bool ExportPredictionsAsCSV(const std::unordered_map<std::string, std::vector<float>>& predictions,
        const std::string& filename);
}