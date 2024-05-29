#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <thread>
#include "camera.h"
#include "camera_parameters.h"
#include "utils.h"
#include "tracking.h"

// Function to process a camera's frame
void processCameraFrame(Camera& camera, int frame_index, int cameras_num) {
    camera.readNextFrame();
    if (camera.currentFrame.empty()) {
        std::cerr << "Error: Read an empty frame for camera: " << camera.name << std::endl;
        return;
    }
    trackBallInFrame(camera, frame_index, cameras_num);
}

void Initialize_cameras_parameters(std::vector<CameraData>& cameraParams, std::vector<Camera>& cameras) {
    int index = 1;
    for (const auto& param : cameraParams) {
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
    int video_length = static_cast<int>(cameras[0].capture.get(cv::CAP_PROP_FRAME_COUNT)); // testing with 20 frames need to be removed
    
    int cameras_num = static_cast<int>(cameras.size());

    std::cout << "Length of the video: " << video_length << std::endl;

    std::vector<std::thread> threads(cameras_num);

    std::ofstream myfile;
    myfile.open("D:/temp/ar51test/csv_files/ball_pos_real.csv");

    for (int frame_index = 0; frame_index < video_length; frame_index++) 
    {
        // Create a thread for each camera to process its current frame
        for (int i = 0; i < cameras_num; ++i) {
            threads[i] = std::thread(processCameraFrame, std::ref(cameras[i]), frame_index, cameras_num);
        }

        // Wait for all threads to complete
        for (auto& thread : threads) {
            if (thread.joinable()) {
                thread.join();
            }
        }

        // Extract 2D points from all cameras
        std::vector<cv::Point2d> imagePoints;
        for (auto& camera : cameras) {
            imagePoints.push_back(camera.current_tracker_position);
        }

        // Perform triangulation
        try {
            cv::Point3d point3D = triangulatePoint(cameras, imagePoints);
            std::cout << "3D Coordinates of the ball at frame " << frame_index << ": " << point3D << std::endl;
            //save the 3d point into a csv file
            
            
            myfile << point3D.x << "," << point3D.y << "," << point3D.z << "\n";
            

        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }

        // Print current position from all cameras
        for (auto& camera : cameras) {
            visualizeOutput(camera);
        }
    }
    myfile.close();

    // Release all videos
    for (auto& camera : cameras) {
        camera.releaseVideo();
    }

    std::cout << "All videos processed successfully." << std::endl;

    return 0;
}
