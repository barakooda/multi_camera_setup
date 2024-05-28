#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

// Function to initialize videos for each camera
void initializeCameraVideos(std::vector<Camera>& cameras, const std::string& videoBasePath) {
    for (auto& camera : cameras) {
        std::string videoPath = videoBasePath + camera.name + ".mp4";
        if (camera.openVideo(videoPath)) {
            std::cout << "Initialized video for camera: " << camera.name << std::endl;
        } else {
            std::cerr << "Error: Failed to open video file for camera: " << camera.name << std::endl;
        }
    }
}

// Function to set backgrounds for each camera
void setCameraBackgrounds(std::vector<Camera>& cameras, const std::string& backgroundPath) {
    for (auto& camera : cameras) {
        std::string backgroundFilePath = backgroundPath + camera.name + "_background.png";
        cv::Mat background = cv::imread(backgroundFilePath);

        if (background.empty()) {
            std::cerr << "Error: Background image not loaded correctly for camera: " << camera.name << std::endl;
        } else {
            camera.setBackground(background);
            // Debug output for background image size
            std::cout << "Loaded background for camera: " << camera.name << " of size: " << background.size() << std::endl;
        }
    }
}
