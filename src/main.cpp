#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <thread>
#include "camera.h"
#include "camera_parameters.h"
#include "utils.h"
#include "tracking.h"

//build command
//cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
//cmake --build build --config Release --target ALL_BUILD -j 18

// Function to process a camera's frame
void processCameraFrame(Camera& camera, int frame_index, int cameras_num) {
    camera.readNextFrame();
    if (camera.currentFrame.empty()) {
        std::cerr << "Error: Read an empty frame for camera: " << camera.name << std::endl;
        return;
    }
    trackBallInFrame(camera, frame_index, cameras_num);
}

// Function to initialize cameras from loaded parameters
void Initialize_cameras_parameters(std::vector<CameraData>& cameraParams, std::vector<Camera>& cameras) {
    int index = 1;
    for (const auto& param : cameraParams) {
        cameras.emplace_back(param.name, param.tvec, param.rvec, param.K, index);
        index++;
    }
}

void processParallelCameraFrames(std::vector<Camera> &cameras,int &cameras_num, int video_length)
{

    // Open a csv file to save the 3d points
    std::ofstream myfile;
    myfile.open("D:/temp/ar51test/csv_files/ball_pos_real.csv");
    
    // Create thread for each camera
    std::vector<std::thread> threads(cameras_num);
    // Process each frame of the video
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

        // Get 2D points from all cameras
        std::vector<cv::Point2d> imagePoints;
        for (auto& camera : cameras) {
            imagePoints.push_back(camera.current_tracker_position);
        }

        // Perform triangulation
        cv::Point3d point3D = triangulatePoint(cameras, imagePoints);
        
        // Save the 3D point to a CSV file
        myfile << point3D.x << "," << point3D.y << "," << point3D.z << "\n";

        
        // Visualize the output
        for (auto& camera : cameras) {
            visualizeOutput(camera);
        }
        

        
        // Debug output
        std::cout << "position at frame " << frame_index << ": " << point3D << std::endl;
        for (auto& camera : cameras) {
            std::cout << "Camera " << camera.index << " is tracking active: " << camera.is_detection_active << std::endl;
        }
        
    }
    myfile.close();
    
    std::cout << "The process is done successfully" << std::endl;
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
    
    // Get number of cameras
    int cameras_num = static_cast<int>(cameras.size());

   processParallelCameraFrames(cameras,cameras_num, video_length);



    return 0;
}


