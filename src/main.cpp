#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include "multi_camera_setup/camera.h"
#include "multi_camera_setup/camera_parameters.h"
#include "multi_camera_setup/utils.h"
#include "multi_camera_setup/tracking.h"
#include <filesystem>

// Function to process a camera's frame
void processCameraFrame(Camera& camera, int frame_index, int cameras_num) {
    camera.readNextFrame();
    if (!camera.current_frame.empty()) {
        trackBallInFrame(camera, frame_index, cameras_num);
    }
}

// Function to initialize cameras from loaded parameters
void Initialize_cameras_parameters(std::vector<CameraData>& cameraParams, std::vector<Camera>& cameras) {
    int index = 1;
    for (const auto& param : cameraParams) {
        cameras.emplace_back(param.name, param.tvec, param.rvec, param.K, index);
        index++;
    }
}

void processParallelCameraFrames(std::vector<Camera>& cameras, int cameras_num, int video_length) {
    std::filesystem::path project_path = findProjectRoot(PROJECT_NAME);
    std::string csvFilePath = (project_path / "csv_files" / "ball_pos_real.csv").string();
    std::ofstream myfile(csvFilePath);

    tbb::blocked_range<size_t> range(0, cameras.size());
    std::vector<cv::Point2d> imagePoints(cameras.size());

    for (int frame_index = 0; frame_index < video_length; frame_index++) {
        tbb::parallel_for(range, [&](const tbb::blocked_range<size_t>& range) {
            for (size_t i = range.begin(); i != range.end(); ++i) {
                processCameraFrame(cameras[i], frame_index, cameras_num);
            }
        });

        for (size_t i = 0; i < cameras.size(); ++i) {
            imagePoints[i] = cameras[i].current_tracker_position;
        }

        cv::Point3d point3D = triangulatePoint(cameras, imagePoints);
        myfile << point3D.x << "," << point3D.y << "," << point3D.z << "\n";

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
}

int main() {
    std::filesystem::path project_path = findProjectRoot(PROJECT_NAME);

    std::string jsonFilePath = (project_path / "calibration" / "cameras.json").string();
    std::vector<CameraData> cameraParams = loadCameraParamsFromJson(jsonFilePath);

    std::vector<Camera> cameras;
    Initialize_cameras_parameters(cameraParams, cameras);

    std::string videoBasePath = (project_path / "videos/").string();
    std::string backgroundPath = videoBasePath + "background/";

    initializeCameraVideos(cameras, videoBasePath);
    setCameraBackgrounds(cameras, backgroundPath);

    int video_length = static_cast<int>(cameras[0].capture.get(cv::CAP_PROP_FRAME_COUNT));
    int cameras_num = static_cast<int>(cameras.size());

    processParallelCameraFrames(cameras, cameras_num, video_length);

    return 0;
}
