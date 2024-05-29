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
    cv::Point2f tracker_speed; // 2D speed of the ball
    int index; // Index of the camera
    bool is_tracking_active = false;

    Camera(const std::string& name, 
           const std::vector<double>& tvec, 
           const std::vector<double>& rvec, 
           const std::vector<std::vector<double>>& K,
           const int index)
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

// Method to get projection matrix
    cv::Mat getProjectionMatrix() const {
        cv::Mat R;
        cv::Rodrigues(rvec, R); // Convert rotation vector to rotation matrix
        int rows = static_cast<int>(K.size());
        int cols = static_cast<int>(K[0].size());
        cv::Mat K_mat(rows, cols, CV_64F);
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                K_mat.at<double>(i, j) = K[i][j];
            }
        }
        cv::Mat t_mat(tvec);
        cv::Mat Rt;
        cv::hconcat(R, t_mat, Rt); // [R|t]
        return K_mat * Rt; // K * [R|t]
    }

};

#endif // CAMERA_H
