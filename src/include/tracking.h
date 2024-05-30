#include <opencv2/opencv.hpp>
#include <iostream>
#include "camera.h"
#include "utils.h"

using namespace cv;
using namespace std;

void calculateCurrentPosition(cv::Mat &mask, cv::Mat &frame, Camera &camera)
{
    float areaThreshold = 50.0f;
    vector<Point> contour = findContoursInMask(mask, areaThreshold);

    if (!contour.empty())
    {
        {
            get_PositionFromContour(frame, contour, camera.current_tracker_position, camera.previous_tracker_position);
        }
        
    }
    else
    {
        cout << "previous tracker position: " << camera.previous_tracker_position << endl;
    }
}

void calculateTrackerPositionSpeed(Camera &camera, cv::Mat &frame)
{
    camera.tracker_speed = camera.current_tracker_position - camera.previous_tracker_position;
    visualizeSpeed(camera.previous_tracker_position, camera.current_tracker_position, frame);
}

void tracker_by_detection(Camera &camera)
{
    
    Mat &frame = camera.currentFrame;
    Mat &background = camera.background;

    if (frame.empty() || background.empty())
    {
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

    if (mask.empty())
    {
        cerr << "Error: Mask is empty." << endl;
        return;
    }

    calculateCurrentPosition(mask, frame, camera);

    calculateTrackerPositionSpeed(camera, frame);

    camera.previous_tracker_position = camera.current_tracker_position;
}


void trackBallInFrame(Camera &camera, int frame_index, int cameras_num)
{

    //active detection flag for each camera based on the frame index and camera index for future use.
    camera.is_detection_active = check_detection_active(frame_index, cameras_num, camera);

        tracker_by_detection(camera);

        //Was not able to get good results from the optical flow and kalman filter.
        //TODO learn both to complete the task.

        /*
        First frame: detection in all cameras.
        2nd frame: detection in camera 1, optical flow all cameras.
        3nd frame: detection in camera 2, optical flow all cameras.
        4nd frame: detection in camera 3, optical flow all cameras.
        5nd frame: detection in camera 4, optical flow all cameras.
        …
        N+1 frame: detection in camera Modulo(N,4), optical flow all cameras.
        When a detection and kalman is performed you need to combine the two using data fusion.
            Data Fusion:
        ●	Use or implement a filter (e.g. Kalman) to combine&integrate the data from the 
        optical flow and the detection system, primarily for frames where both data types are available.

        */
   

}


