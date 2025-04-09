// FileIO.cpp
// Implementation of file I/O utilities
#include "pch.h"
#include "FileIO.h"

namespace FileIO {

    bool SaveRouteToFile(const Route* route, const std::string& filename)
    {
        // Open file for writing
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Failed to open file: " << filename << std::endl;
            return false;
        }

        // Get current time
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);

        // Write header
        file << "=====================================================" << std::endl;
        file << "Waste Management System - Route Report" << std::endl;
        file << "=====================================================" << std::endl;
        file << "Date: " << std::ctime(&time);
        file << "Route Type: " << route->GetRouteName() << std::endl;
        file << "Waste Threshold: " << route->GetWasteThreshold() << "%" << std::endl;
        file << "=====================================================" << std::endl;

        // Write route data
        const std::vector<int>& routePath = route->GetFinalRoute();
        const std::vector<float>& distances = route->GetIndividualDistances();

        // Write route sequence
        file << "Route Sequence: ";
        for (size_t i = 0; i < routePath.size(); i++) {
            file << WasteLocation::dict_Id_to_Name[routePath[i]];
            if (i < routePath.size() - 1) {
                file << " -> ";
            }
        }
        file << std::endl << std::endl;

        // Write segment distances
        file << "Segment Distances:" << std::endl;
        file << "-----------------------------------------------------" << std::endl;
        file << "From\tTo\tDistance (km)" << std::endl;
        file << "-----------------------------------------------------" << std::endl;

        for (size_t i = 0; i < routePath.size() - 1; i++) {
            file << WasteLocation::dict_Id_to_Name[routePath[i]] << "\t"
                << WasteLocation::dict_Id_to_Name[routePath[i + 1]] << "\t"
                << distances[i] << std::endl;
        }
        file << std::endl;

        // Write cost summary
        file << "Cost Summary:" << std::endl;
        file << "-----------------------------------------------------" << std::endl;
        file << "Total Distance: " << route->GetTotalDistance() << " km" << std::endl;
        file << "Time Taken: " << route->GetTimeTaken() << " min ("
            << route->GetTimeTaken() / 60.0f << " hours)" << std::endl;
        file << "Fuel Consumption: RM " << route->GetFuelConsumption() << std::endl;
        file << "Driver's Wage: RM " << route->GetWage() << std::endl;
        file << "Total Cost: RM " << route->GetTotalCost() << std::endl;

        file.close();

        std::cout << "Route saved to: " << filename << std::endl;

        return true;
    }

    bool SaveWasteLocationsToFile(const std::vector<WasteLocation>& locations, const std::string& filename)
    {
        // Open file for writing
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Failed to open file: " << filename << std::endl;
            return false;
        }

        // Get current time
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);

        // Write header
        file << "=====================================================" << std::endl;
        file << "Waste Management System - Waste Levels" << std::endl;
        file << "=====================================================" << std::endl;
        file << "Date: " << std::ctime(&time);
        file << "=====================================================" << std::endl;

        // Write waste levels
        file << "Location\tWaste Level (%)" << std::endl;
        file << "-----------------------------------------------------" << std::endl;

        for (const auto& location : locations) {
            file << location.GetLocationName() << "\t\t"
                << location.GetWasteLevel() << std::endl;
        }

        file.close();

        std::cout << "Waste locations saved to: " << filename << std::endl;

        return true;
    }

    bool LoadWasteLocationsFromFile(const std::string& filename, std::vector<WasteLocation>& locations)
    {
        // Open file for reading
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Failed to open file: " << filename << std::endl;
            return false;
        }

        // Clear existing locations
        locations.clear();

        // Skip header lines until "-----"
        std::string line;
        while (std::getline(file, line)) {
            if (line.find("-----") != std::string::npos) {
                break;
            }
        }

        // Skip column headers
        std::getline(file, line);

        // Read location data
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string name;
            float wasteLevel;

            if (iss >> name >> wasteLevel) {
                WasteLocation location(name);
                location.SetWasteLevel(wasteLevel);
                locations.push_back(location);
            }
        }

        file.close();

        std::cout << "Loaded " << locations.size() << " waste locations from: " << filename << std::endl;

        return true;
    }

    bool SaveSimulationReport(const std::vector<Route*>& routes,
        const std::vector<WasteLocation>& locations,
        const std::string& filename)
    {
        // Open file for writing
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Failed to open file: " << filename << std::endl;
            return false;
        }

        // Get current time
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);

        // Write header
        file << "=====================================================" << std::endl;
        file << "Waste Management System - Simulation Report" << std::endl;
        file << "=====================================================" << std::endl;
        file << "Date: " << std::ctime(&time);
        file << "=====================================================" << std::endl;

        // Write waste levels
        file << "Waste Levels:" << std::endl;
        file << "-----------------------------------------------------" << std::endl;
        file << "Location\tWaste Level (%)" << std::endl;
        file << "-----------------------------------------------------" << std::endl;

        for (const auto& location : locations) {
            file << location.GetLocationName() << "\t\t"
                << location.GetWasteLevel() << std::endl;
        }

        file << std::endl;

        // Write route comparison
        file << "Route Comparison Summary:" << std::endl;
        file << "-----------------------------------------------------" << std::endl;
        file << "Route Type\tWaste Threshold\tTotal Distance\tTotal Cost" << std::endl;
        file << "-----------------------------------------------------" << std::endl;

        for (const auto& route : routes) {
            file << route->GetRouteName() << "\t"
                << route->GetWasteThreshold() << "%\t\t"
                << route->GetTotalDistance() << " km\t\t"
                << "RM " << route->GetTotalCost() << std::endl;
        }

        file << std::endl;

        // Write detailed route information for each route
        for (const auto& route : routes) {
            file << "=====================================================" << std::endl;
            file << "Route Type: " << route->GetRouteName() << std::endl;
            file << "Waste Threshold: " << route->GetWasteThreshold() << "%" << std::endl;
            file << "=====================================================" << std::endl;

            const std::vector<int>& routePath = route->GetFinalRoute();
            const std::vector<float>& distances = route->GetIndividualDistances();

            // Write route sequence
            file << "Route Sequence: ";
            for (size_t i = 0; i < routePath.size(); i++) {
                file << WasteLocation::dict_Id_to_Name[routePath[i]];
                if (i < routePath.size() - 1) {
                    file << " -> ";
                }
            }
            file << std::endl << std::endl;

            // Write segment distances
            file << "Segment Distances:" << std::endl;
            file << "-----------------------------------------------------" << std::endl;

            for (size_t i = 0; i < routePath.size() - 1; i++) {
                file << WasteLocation::dict_Id_to_Name[routePath[i]] << " -> "
                    << WasteLocation::dict_Id_to_Name[routePath[i + 1]] << ": "
                    << distances[i] << " km" << std::endl;
            }
            file << std::endl;

            // Write cost summary
            file << "Cost Summary:" << std::endl;
            file << "-----------------------------------------------------" << std::endl;
            file << "Total Distance: " << route->GetTotalDistance() << " km" << std::endl;
            file << "Time Taken: " << route->GetTimeTaken() << " min ("
                << route->GetTimeTaken() / 60.0f << " hours)" << std::endl;
            file << "Fuel Consumption: RM " << route->GetFuelConsumption() << std::endl;
            file << "Driver's Wage: RM " << route->GetWage() << std::endl;
            file << "Total Cost: RM " << route->GetTotalCost() << std::endl;

            file << std::endl;
        }

        file.close();

        std::cout << "Simulation report saved to: " << filename << std::endl;

        return true;
    }

    bool ExportRouteAsCSV(const Route* route, const std::string& filename)
    {
        // Open file for writing
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Failed to open file: " << filename << std::endl;
            return false;
        }

        // Write header
        file << "RouteType,WasteThreshold,Location,Order,DistanceFromPrevious" << std::endl;

        // Write route data
        const std::vector<int>& routePath = route->GetFinalRoute();
        const std::vector<float>& distances = route->GetIndividualDistances();

        for (size_t i = 0; i < routePath.size(); i++) {
            file << route->GetRouteName() << ","
                << route->GetWasteThreshold() << ","
                << WasteLocation::dict_Id_to_Name[routePath[i]] << ","
                << i;

            if (i > 0) {
                file << "," << distances[i - 1];
            }
            else {
                file << ",0";
            }

            file << std::endl;
        }

        file.close();

        std::cout << "Route exported as CSV to: " << filename << std::endl;

        return true;
    }

    bool ExportWasteLevelsAsCSV(const std::vector<WasteLocation>& locations, const std::string& filename)
    {
        // Open file for writing
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Failed to open file: " << filename << std::endl;
            return false;
        }

        // Write header
        file << "Location,WasteLevel" << std::endl;

        // Write waste levels
        for (const auto& location : locations) {
            file << location.GetLocationName() << ","
                << location.GetWasteLevel() << std::endl;
        }

        file.close();

        std::cout << "Waste levels exported as CSV to: " << filename << std::endl;

        return true;
    }

    bool ExportPredictionsAsCSV(const std::unordered_map<std::string, std::vector<float>>& predictions,
        const std::string& filename)
    {
        // Open file for writing
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Failed to open file: " << filename << std::endl;
            return false;
        }

        // Find maximum number of days in predictions
        size_t maxDays = 0;
        for (const auto& pair : predictions) {
            maxDays = std::max(maxDays, pair.second.size());
        }

        // Write header
        file << "Location,CurrentWasteLevel";
        for (size_t i = 1; i < maxDays; i++) {
            file << ",Day" << i;
        }
        file << std::endl;

        // Write predictions
        for (const auto& pair : predictions) {
            if (pair.first == "Station") {
                continue; // Skip station
            }

            file << pair.first;

            for (size_t i = 0; i < pair.second.size(); i++) {
                file << "," << pair.second[i];
            }

            // Fill remaining days with empty values
            for (size_t i = pair.second.size(); i < maxDays; i++) {
                file << ",";
            }

            file << std::endl;
        }

        file.close();

        std::cout << "Predictions exported as CSV to: " << filename << std::endl;

        return true;
    }

} // namespace FileIO