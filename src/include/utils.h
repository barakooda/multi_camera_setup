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
        cv::resize(camera.currentFrame, resized_image, cv::Size(690, 512), 0, 0, cv::INTER_AREA);

        imshow(window_name, resized_image);
        resizeWindow(window_name, 690, 512);
        moveWindow(window_name, x_pos, y_pos);
        waitKey(1);
}

// Function to check if the tracking is active
bool check_detection_active(int frame_index, int cameras_num, Camera &camera)
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

cv::Point2f get_PositionFromContour(Mat &frame, vector<Point> &contour, Point2f &tracker_pos, Point2f &previous_tracker_pos) 
{
    float radius;
    // Get the minimum enclosing circle
    minEnclosingCircle(contour, tracker_pos, radius);
    
    /*
    // Draw the circle
    cv::Point tracker_pos_int = cv::Point(cvRound(tracker_pos.x), cvRound(tracker_pos.y));;
    circle(frame, tracker_pos_int, cvRound(radius), Scalar(0, 255, 0), 2); 
    */
   
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

    // Accumulator for homogeneous points
    cv::Mat points4D = cv::Mat::zeros(4, 1, CV_64F);

    // Convert cv::Point2d to cv::Mat
    std::vector<cv::Mat> imagePointsMat;
    for (const auto& point : imagePoints) {
        cv::Mat ptMat(2, 1, CV_64F);
        ptMat.at<double>(0) = point.x;
        ptMat.at<double>(1) = point.y;
        imagePointsMat.push_back(ptMat);
    }

    // Triangulate points using all pairs of cameras
    int pair_count = 0;
    for (size_t i = 0; i < cameras.size() - 1; ++i) {
        for (size_t j = i + 1; j < cameras.size(); ++j) {
            cv::Mat points4DNew;
            cv::triangulatePoints(projectionMatrices[i], projectionMatrices[j], imagePointsMat[i], imagePointsMat[j], points4DNew);
            points4D += points4DNew;
            pair_count++;
        }
    }
    points4D /= pair_count;

    // Convert from homogeneous coordinates to 3D
    cv::Point3d point3D(
        points4D.at<double>(0) / points4D.at<double>(3),
        points4D.at<double>(1) / points4D.at<double>(3),
        points4D.at<double>(2) / points4D.at<double>(3)
    );

    return point3D;
}


cv::Point2f trackPointOpticalFlow(const cv::Mat& previousFrame, const cv::Mat& currentFrame, const cv::Point2f& previousPoint) {
    std::vector<cv::Point2f> previousPoints(1, previousPoint);
    std::vector<cv::Point2f> currentPoints;
    std::vector<uchar> status;
    std::vector<float> err;

    // Parameters for cv::calcOpticalFlowPyrLK
    cv::Size winSize = cv::Size(128, 128);
    int maxLevel = 2;
    cv::TermCriteria criteria = cv::TermCriteria(cv::TermCriteria::EPS | cv::TermCriteria::COUNT, 10, 0.03);

    // Calculate optical flow to get the new point position
    cv::calcOpticalFlowPyrLK(previousFrame, currentFrame, previousPoints, currentPoints, status, err, winSize, maxLevel, criteria);

    // Check if the flow was found
    if (status[0] == 1) {
        return currentPoints[0];
    } else {
        return previousPoint;
    }
}


#endif