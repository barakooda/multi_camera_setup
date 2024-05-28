#ifndef CAMERA_H
#define CAMERA_H

#include <string>
#include <opencv2/opencv.hpp>

class Camera {
public:
    std::string name;
    std::vector<double> tvec; // Translation vector
    std::vector<double> rvec; // Rotation vector
    std::vector<std::vector<double>> K; // Intrinsic matrix
    cv::Mat currentFrame; // Current video frame
    cv::Mat background; // Background image
    cv::VideoCapture capture; // Video capture object
    cv::Point2f current_tracker_position; // Center of the ball
    cv::Point2f previous_tracker_position; // Previous center of the ball
    unsigned int index; // Index of the camera

    Camera(const std::string& name, 
           const std::vector<double>& tvec, 
           const std::vector<double>& rvec, 
           const std::vector<std::vector<double>>& K,
           const unsigned int index)
    : name(name), tvec(tvec), rvec(rvec), K(K),index(index) {}

    // Method to open video file
    bool openVideo(const std::string& videoPath) {
        capture.open(videoPath);
        if (!capture.isOpened()) {
            std::cerr << "Failed to open video file: " << videoPath << std::endl;
            return false;
        }
        return true;
    }

    // Method to read the next frame from the video
    bool readNextFrame() {
        if (!capture.isOpened()) {
            std::cerr << "Video file not opened." << std::endl;
            return false;
        }
        return capture.read(currentFrame);
    }

    // Release the video capture object
    void releaseVideo() {
        if (capture.isOpened()) {
            capture.release();
        }
    }

    // Method to set the background image
    void setBackground(const cv::Mat& bg) {
        background = bg.clone();
    }
};

#endif // CAMERA_H
