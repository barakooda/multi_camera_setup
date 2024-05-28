#include <opencv2/opencv.hpp>
#include <iostream>
#include "camera.h"

using namespace cv;
using namespace std;

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

void trackBallInFrame(Camera& camera) {
    Mat& frame = camera.currentFrame;
    Mat& background = camera.background;

    if (frame.empty() || background.empty()) {
        cerr << "Error: Frame or background is empty." << endl;
        return;
    }

    // HSV range for the pink ball
    Scalar lower_pink(130, 50, 50);
    Scalar upper_pink(180, 255, 255);

    Mat mask, diff, foregroundMask, hsvFrame;

    // Convert frame to HSV
    cvtColor(frame, hsvFrame, COLOR_BGR2HSV);

    // Background subtraction
    absdiff(frame, background, diff);
    cvtColor(diff, diff, COLOR_BGR2GRAY);
    threshold(diff, foregroundMask, 50, 255, THRESH_BINARY);

    // Color keying in HSV
    inRange(hsvFrame, lower_pink, upper_pink, mask);

    // Combine the masks
    bitwise_and(mask, foregroundMask, mask);

    // Apply some preprocessing (e.g., dilate and erode to clean up the mask)
    dilate(mask, mask, Mat(), Point(-1, -1), 2);
    erode(mask, mask, Mat(), Point(-1, -1), 2);

    if (mask.empty()) {
        cerr << "Error: Mask is empty." << endl;
        return;
    }

    float areaThreshold = 50.0f;
    vector<vector<Point>> contours = findContoursInMask(mask, areaThreshold);
    if (!contours.empty()) {
        {
            drawCirclesForContours(frame, contours, camera.current_tracker_position, camera.previous_tracker_position);
        }

        visualizeOpticalFlow(camera.previous_tracker_position, camera.current_tracker_position, frame);

        camera.previous_tracker_position = camera.current_tracker_position;
        
    } else {
        camera.previous_tracker_position = Point2f(-1, -1);
        cout << "center: " << camera.previous_tracker_position << endl;
    }
}
