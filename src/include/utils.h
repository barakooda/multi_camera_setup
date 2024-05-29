#ifndef UTILS_H
#define UTILS_H

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


bool check_tracking_active(int frame_index, int cameras_num, Camera &camera)
{
    // std::cout << "Camera index: " << camera.index << std::endl;
    // std::cout << "Processing frame " << frame_index << std::endl;
    // std::cout << "Number of cameras: " << cameras_num << std::endl;

    int active_index = frame_index % cameras_num;
    if (active_index == 0)
    {
        active_index = cameras_num;
    }
    return (camera.index == active_index);

    // std::cout << "is_tracking_active: " << is_tracking_active << std::endl<< std::endl;
}


vector<vector<Point>> findContoursInMask(const Mat &mask, float areaThreshold) {
    vector<vector<Point>> contours;
    findContours(mask, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    vector<vector<Point>> filteredContours;
    for (const auto &contour : contours) {
        if (contourArea(contour) > areaThreshold) {
            filteredContours.push_back(contour);
        }
    }

    return filteredContours;
}

void drawCirclesForContours(Mat &frame, const vector<vector<Point>> &contours, Point2f &tracker_pos, Point2f &previous_tracker_pos) {
    for (const auto &contour : contours) {
        float radius;
        minEnclosingCircle(contour, tracker_pos, radius);
        circle(frame, tracker_pos, static_cast<int>(radius), Scalar(0, 255, 0), 2);
        return;
    }

    previous_tracker_pos = Point2f(-1, -1);
    //cout << "center: " << previous_tracker_pos << endl;
}

void visualizeOpticalFlow(const Point2f &prev_point, const Point2f &curr_point, Mat &frame) {
    // Calculate the direction vector
    Point2f direction = curr_point - prev_point;
    
    // Scale the direction vector
    float scaleFactor = 50; // Adjust this factor to make the line longer
    Point2f extended_point = prev_point + direction * scaleFactor;
    
    // Draw the arrowed line
    arrowedLine(frame, prev_point, extended_point, Scalar(0, 255, 0), 5, 8, 0, 0.3);
    //cout << "Optical Flow: from " << prev_point << " to " << extended_point << endl;
}

#endif