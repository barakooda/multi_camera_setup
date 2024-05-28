#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

void trackBallInFrame(Camera& camera) {

    auto frame = camera.currentFrame;
    auto background = camera.background;
    auto tracker_pos = camera.current_tracker_position;
    auto previous_tracker_pos = camera.previous_tracker_position;

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

    // Find contours to get initial bounding box
    vector<vector<Point>> contours;
    findContours(mask, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    if (contours.empty()) {
        return;
    }

    float areaThreshold = 50.0f;

    // Draw circles for contours above the area threshold
    for (size_t i = 0; i < contours.size(); i++) {
        double area = contourArea(contours[i]);
        if (area > areaThreshold) {
            float radius;
            minEnclosingCircle(contours[i], tracker_pos, radius);
            circle(frame, tracker_pos, static_cast<int>(radius), Scalar(0, 255, 0), 2);
            // Print tracker position
            cout << "center: " << tracker_pos << endl;
            previous_tracker_pos = tracker_pos;
            break;
        }
        else
        {
            
            previous_tracker_pos = Point2f(-1, -1);
            cout << "center: " << previous_tracker_pos << endl;
        }
    }
}
