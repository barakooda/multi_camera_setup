#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <opencv2/opencv.hpp>

#include <filesystem>
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

//function to visualize the output
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
        cv::resize(camera.current_frame, resized_image, cv::Size(690, 512), 0, 0, cv::INTER_AREA);

        imshow(window_name, resized_image);
        resizeWindow(window_name, 690, 512);
        moveWindow(window_name, x_pos, y_pos);
        waitKey(1);
}

// Function to check if the tracking is active
bool checkDetectionActive(int frame_index, int cameras_num, Camera &camera)
{

    int active_index = frame_index % cameras_num;
    if (active_index == 0)
    {
        active_index = cameras_num;
    }
    return (camera.index == active_index);

}


vector<Point> findContoursInMask(const Mat &mask, float areaThreshold) {
    vector<vector<Point>> contours;
    findContours(mask, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    vector<Point> filteredContour;

    for (const auto &contour : contours) {
        if (contourArea(contour) > areaThreshold) {
            filteredContour = contour;
            break;
        }
    }

    return filteredContour;
}

cv::Point2f getPositionFromContour(Mat &frame, vector<Point> &contour, Point2f &tracker_pos, Point2f &previous_tracker_pos) 
{
    float radius;
    // Get the minimum enclosing circle
    minEnclosingCircle(contour, tracker_pos, radius);
    
    
    // Draw the circle
    cv::Point tracker_pos_int = cv::Point(cvRound(tracker_pos.x), cvRound(tracker_pos.y));;
    circle(frame, tracker_pos_int, cvRound(radius), Scalar(0, 255, 0), 2); 
    
   
    return tracker_pos;
}


void visualizeSpeed(const Point2f &prev_point, const Point2f &curr_point, Mat &frame) {
    // Calculate the direction vector
    Point2f direction = curr_point - prev_point;
    
    // Scale the direction vector
    float scaleFactor = 50; // Adjust this factor to make the line longer
    Point2f extended_point = prev_point + direction * scaleFactor;
    
    // Draw the arrowed line
    arrowedLine(frame, prev_point, extended_point, Scalar(0, 255, 0), 5, 8, 0, 0.3);
    //cout << "Optical Flow: from " << prev_point << " to " << extended_point << endl;
}


cv::Point3d triangulatePoint(const std::vector<Camera>& cameras, const std::vector<cv::Point2d>& imagePoints) {
    if (cameras.size() != imagePoints.size()) {
        throw std::invalid_argument("Number of cameras must match the number of image points.");
    }

    std::vector<cv::Mat> projectionMatrices;
    for (const auto& camera : cameras) {
        projectionMatrices.push_back(camera.getProjectionMatrix());
    }

    // Matrix to hold the linear system of equations
    cv::Mat A = cv::Mat::zeros(2 * (int)cameras.size(), 4, CV_64F);

    // Fill the matrix A
    for (int i = 0; i < (int)cameras.size(); ++i) {
        double x = imagePoints[i].x;
        double y = imagePoints[i].y;
        const cv::Mat& P = projectionMatrices[i];

        A.row(2 * i)     = x * P.row(2) - P.row(0);
        A.row(2 * i + 1) = y * P.row(2) - P.row(1);
    }

    // Perform SVD
    cv::Mat w, u, vt;
    cv::SVD::compute(A, w, u, vt);

    // The solution is the last row of Vt
    cv::Mat point4D = vt.row(3).t();

    // Convert from homogeneous coordinates to 3D
    cv::Point3d point3D(
        point4D.at<double>(0) / point4D.at<double>(3),
        point4D.at<double>(1) / point4D.at<double>(3),
        point4D.at<double>(2) / point4D.at<double>(3)
    );

    return point3D;
}



cv::Point2f trackPointOpticalFlow(const cv::Mat& previous_frame, const cv::Mat& current_frame, const cv::Point2f& previous_point) {
    std::vector<cv::Point2f> previous_points(1, previous_point);
    std::vector<cv::Point2f> current_points;
    std::vector<uchar> status;
    std::vector<float> err;

    // Parameters for cv::calcOpticalFlowPyrLK
    cv::Size winSize = cv::Size(128, 128);
    int maxLevel = 2;
    cv::TermCriteria criteria = cv::TermCriteria(cv::TermCriteria::EPS | cv::TermCriteria::COUNT, 10, 0.03);

    // Calculate optical flow to get the new point position
    cv::calcOpticalFlowPyrLK(previous_frame, current_frame, previous_points, current_points, status, err, winSize, maxLevel, criteria);

    // Check if the flow was found
    if (status[0] == 1) {
        return current_points[0];
    } else {
        return previous_point;
    }
}


std::filesystem::path findProjectRoot(const std::string& project_name) {
    std::filesystem::path current_path = std::filesystem::current_path();
    while (current_path.has_parent_path()) {
        if (current_path.filename() == project_name) {
            return current_path;
        }
        current_path = current_path.parent_path();
    }
    throw std::runtime_error("Project root not found");
}


#endif