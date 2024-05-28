#include <iostream>
#include <vector>
#include <thread>
#include "include/camera.h"
#include "camera_parameters.h"
#include "utils.h"
#include "tracking.h"

// Function to process a camera's frame
void processCameraFrame(Camera& camera, int index) {
    camera.readNextFrame();
    if (camera.currentFrame.empty()) {
        std::cerr << "Error: Read an empty frame for camera: " << camera.name << std::endl;
        return;
    }
    trackBallInFrame(camera);
}

int main() {
    // Load camera parameters from JSON file
    std::string jsonFilePath = "D:/temp/ar51test/calibration/cameras.json";
    std::vector<CameraData> cameraParams = loadCameraParamsFromJson(jsonFilePath);
    std::vector<Camera> cameras;

    // Initialize cameras from loaded parameters
    unsigned int index = 1;
    for (const auto& param : cameraParams) {
        cameras.emplace_back(param.name, param.tvec, param.rvec, param.K,index);
        index++;
    }

    // Base path for videos and backgrounds
    std::string videoBasePath = "D:/temp/ar51test/videos/";
    std::string backgroundPath = videoBasePath + "background/";

    // Initialize videos for all cameras
    initializeCameraVideos(cameras, videoBasePath);

    // Set backgrounds for all cameras
    setCameraBackgrounds(cameras, backgroundPath);

    // Get the length of the video
    int length = int(cameras[0].capture.get(cv::CAP_PROP_FRAME_COUNT));
    std::cout << "Length of the video: " << length << std::endl;

    std::vector<std::thread> threads(cameras.size());

    for (int index = 0; index < length; index++) {
        // Create a thread for each camera to process its current frame
        for (size_t i = 0; i < cameras.size(); ++i) {
            threads[i] = std::thread(processCameraFrame, std::ref(cameras[i]), index);
        }

        // Wait for all threads to complete
        for (auto& thread : threads) {
            if (thread.joinable()) {
                thread.join();
            }
        }

        // Print the current index
        std::cout << "index: " << index << std::endl;
        for (auto camera : cameras) {
        // Print current position from all cameras
        std::cout << "Camera" << camera.index << ": Current position: " << camera.current_tracker_position << std::endl;
        }
    }

    // Release all videos
    for (auto& camera : cameras) {
        camera.releaseVideo();
    }

    std::cout << "All videos processed successfully." << std::endl;

    return 0;
}
