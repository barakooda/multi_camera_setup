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


void visualizeOutput(Camera &camera)
{
        std::string window_name = "Camera" + std::to_string(camera.index);

        int x_pos = (camera.index - 1) * 690;
        int y_pos = 0;

        if (camera.index > 2) {
            x_pos = (camera.index - 3) * 690;
            y_pos = 512;
        }

        cv::Mat resized_image;
        cv::resize(camera.currentFrame, resized_image, cv::Size(690, 512), 0, 0, cv::INTER_AREA);

        imshow(window_name, resized_image);
        resizeWindow(window_name, 690, 512);
        moveWindow(window_name, x_pos, y_pos);
        waitKey(1);
}
