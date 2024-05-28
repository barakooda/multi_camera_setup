#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>
#include <iostream>

using namespace cv;
using namespace std;

void trackBallInFrame(Mat &frame, const Mat &background) {
    if (frame.empty() || background.empty()) {
        cerr << "Error: Frame or background is empty." << endl;
        return;
    }

    Ptr<Tracker> tracker = TrackerCSRT::create();
    Rect bbox;

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
    
    // Check if the mask is empty
    if (mask.empty()) {
        cerr << "Error: Mask is empty." << endl;
        return;
    }
    
    // Find contours to get initial bounding box
    vector<vector<Point>> contours;
    findContours(mask, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    // Draw contours on the frame
    if (contours.empty())
    {
        return;
    }

    float areaThreshold = 50.0f;

    // Draw circles for contours above the area threshold
    for (size_t i = 0; i < contours.size(); i++) {
        double area = contourArea(contours[i]);
        if (area > areaThreshold) {
            Point2f center;
            float radius;
            minEnclosingCircle(contours[i], center,radius);
            circle(frame, center, static_cast<int>(radius), Scalar(0, 255, 0), 2);
        }
    }
}

    /*
    if (!contours.empty()) {
        bbox = boundingRect(contours[0]);
        if (bbox.width > 0 && bbox.height > 0) {
            tracker->init(frame, bbox);

            // Update the tracker
            bool ok = tracker->update(frame, bbox);
            if (ok) {
                // Tracking success
                rectangle(frame, bbox, Scalar(255, 0, 0), 2, 1);
            } else {
                cerr << "Tracking failed." << endl;
            }
        } else {
            cerr << "Bounding box is empty." << endl;
        }
    } else {
        cerr << "No contours found." << endl;
    }
    */
