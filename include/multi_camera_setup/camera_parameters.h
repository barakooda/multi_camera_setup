#ifndef CAMERA_PARAMETERS_H
#define CAMERA_PARAMETERS_H

#include <fstream>
#include <iostream>
#include <vector>
#include <nlohmann/json.hpp>
#include <opencv2/opencv.hpp>

// Use the nlohmann namespace for convenience
using json = nlohmann::json;

// Define a struct to hold camera parameters
struct CameraData {
    std::string name; // Name of the camera
    std::vector<double> tvec; // Translation vector
    std::vector<double> rvec; // Rotation vector
    std::vector<std::vector<double>> K; // Intrinsic matrix
};

// Function to load camera parameters from a JSON file
std::vector<CameraData> loadCameraParamsFromJson(const std::string& jsonFilePath) {
    std::ifstream fileStream(jsonFilePath);
    if (!fileStream) {
        std::cerr << "Could not open JSON file: " << jsonFilePath << std::endl;
        return {}; // Return an empty vector if file can't be opened
    }

    json jsonData;
    fileStream >> jsonData; // Parse the JSON file

    std::vector<CameraData> cameraParametersList;
    for (const json& cameraItem : jsonData) {
        CameraData cameraData;
        cameraData.name = cameraItem["name"];
        cameraData.tvec = cameraItem["tvec"].get<std::vector<double>>();
        cameraData.rvec = cameraItem["rvec"][0].get<std::vector<double>>(); // Assuming rvec is nested in an array
        
        // Extracting the intrinsic matrix K
        cameraData.K.clear();
        for (const json& row : cameraItem["K"]) {
            cameraData.K.push_back(row.get<std::vector<double>>());
        }

        cameraParametersList.push_back(cameraData);
    }

    return cameraParametersList;
}

#endif // CAMERA_PARAMETERS_H
