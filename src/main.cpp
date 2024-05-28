#include <iostream>
#include <vector>
#include "include/camera.h"
#include "camera_parameters.h"
#include "utils.h"
#include "tracking.h"

int main() {
    try {
        std::string jsonFilePath = "D:/temp/ar51test/calibration/cameras.json";
        std::vector<CameraData> cameraParams = loadCameraParamsFromJson(jsonFilePath);
        std::vector<Camera> cameras;

        // Initialize cameras from loaded parameters
        for (const auto& param : cameraParams) {
            cameras.emplace_back(param.name, param.tvec, param.rvec, param.K);
        }

        // Base path for videos and backgrounds
        std::string videoBasePath = "D:/temp/ar51test/videos/";
        std::string backgroundPath = videoBasePath + "background/";

        // Initialize videos for all cameras
        initializeCameraVideos(cameras, videoBasePath);

        // Set backgrounds for all cameras
        setCameraBackgrounds(cameras, backgroundPath);

        // Process the frames for camera 4
        Camera& camera = cameras[3];

        // Check if background image is loaded correctly
        if (camera.background.empty()) {
            std::cerr << "Error: Background image not loaded correctly for camera: " << camera.name << std::endl;
            return -1;
        }

        // Debug output for background image size
        std::cout << "Background image size: " << camera.background.size() << std::endl;

        // Process each frame
        if (camera.openVideo(videoBasePath + camera.name + ".mp4")) {
            while (camera.readNextFrame()) {
                if (camera.currentFrame.empty()) {
                    std::cerr << "Error: Read an empty frame for camera: " << camera.name << std::endl;
                    continue;
                }

                // Debug output for frame size
                std::cout << "Processing frame of size: " << camera.currentFrame.size() << std::endl;

                trackBallInFrame(camera.currentFrame, camera.background);

                // Display the processed frame
                cv::imshow("Processed Frame", camera.currentFrame);

                // Wait for a short period to display the frame
                if (cv::waitKey(30) >= 0) {
                    break; // Exit if any key is pressed
                }
            }

            camera.releaseVideo();
        } else {
            std::cerr << "Error: Failed to reopen video file for camera: " << camera.name << std::endl;
        }

        std::cout << "All videos processed successfully." << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}
