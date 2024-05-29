#include <iostream>
#include <vector>
#include <thread>
#include "include/camera.h"
#include "camera_parameters.h"
#include "utils.h"
#include "tracking.h"
#include "main.h"

// Function to process a camera's frame
void processCameraFrame(Camera& camera, int frame_index,int cameras_num) {
    camera.readNextFrame();
    if (camera.currentFrame.empty()) {
        std::cerr << "Error: Read an empty frame for camera: " << camera.name << std::endl;
        return;
    }
    trackBallInFrame(camera,frame_index,cameras_num);
}


void Initialize_cameras_parameters(std::vector<CameraData> &cameraParams, std::vector<Camera> &cameras)
{
    int index = 1;
    for (const auto &param : cameraParams)
    {
        cameras.emplace_back(param.name, param.tvec, param.rvec, param.K, index);
        index++;
    }
}


int main() {
    // Load camera parameters from JSON file
    std::string jsonFilePath = "D:/temp/ar51test/calibration/cameras.json";
    std::vector<CameraData> cameraParams = loadCameraParamsFromJson(jsonFilePath);
    std::vector<Camera> cameras;

    // Initialize cameras from loaded parameters
    Initialize_cameras_parameters(cameraParams, cameras);

    // Base path for videos and backgrounds
    std::string videoBasePath = "D:/temp/ar51test/videos/";
    std::string backgroundPath = videoBasePath + "background/";

    // Initialize videos for all cameras
    initializeCameraVideos(cameras, videoBasePath);

    // Set backgrounds for all cameras
    setCameraBackgrounds(cameras, backgroundPath);

    // Get the length of the video
    int video_length = int(cameras[0].capture.get(cv::CAP_PROP_FRAME_COUNT)) / 20;
    
    int cameras_num = int(cameras.size());

    std::cout << "Length of the video: " << video_length << std::endl;

    std::vector<std::thread> threads(cameras_num);

    for (int frame_index = 0; frame_index < video_length; frame_index++) {
        // Create a thread for each camera to process its current frame
        for (int i = 0; i < cameras_num; ++i) {
            threads[i] = std::thread(processCameraFrame, std::ref(cameras[i]), frame_index,cameras_num);
        }

        // Wait for all threads to complete
        for (auto& thread : threads) {
            if (thread.joinable()) {
                thread.join();
            }
        }

        // Print the current index
        std::cout << "frame index: " << frame_index << std::endl;
        
        for (auto camera : cameras) {
            // Print current position from all cameras
            std::cout << "Camera" << camera.index << ": Is tracker active " << camera.is_tracking_active << std::endl;
        
            visualizeOutput(camera);
        }
    }

    // Release all videos
    for (auto& camera : cameras) {
        camera.releaseVideo();
    }

    std::cout << "All videos processed successfully." << std::endl;

    return 0;
}


