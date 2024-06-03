#ifndef SIMPLEKALMANFILTER_H
#define SIMPLEKALMANFILTER_H

#include <opencv2/opencv.hpp>
#include <iostream>

class SimpleKalmanFilter {
public:
    SimpleKalmanFilter() {
        initKalmanFilter();
    }

    void initKalmanFilter() {
        // Create Kalman Filter with 4 dynamic parameters and 2 measurement parameters
        KF.init(4, 2, 0, CV_32F);

        // Transition matrix (A)
        KF.transitionMatrix = (cv::Mat_<float>(4, 4) << 1, 0, 1, 0,
                                                        0, 1, 0, 1,
                                                        0, 0, 1, 0,
                                                        0, 0, 0, 1);

        // Measurement matrix (H)
        KF.measurementMatrix = (cv::Mat_<float>(2, 4) << 1, 0, 0, 0,
                                                         0, 1, 0, 0);

        // Adjust the process noise covariance (Q) to be more responsive
        cv::setIdentity(KF.processNoiseCov, cv::Scalar::all(1e-2));

        // Adjust the measurement noise covariance (R) to be more responsive
        cv::setIdentity(KF.measurementNoiseCov, cv::Scalar::all(1e-2));

        // Error covariance (P)
        cv::setIdentity(KF.errorCovPost, cv::Scalar::all(1));

        // Initialize state
        KF.statePost = cv::Mat::zeros(4, 1, CV_32F);
        measurement = cv::Mat::zeros(2, 1, CV_32F);
    }

    cv::Point2f predict() {
        cv::Mat prediction = KF.predict();
        return cv::Point2f(prediction.at<float>(0), prediction.at<float>(1));
    }

    void correct(cv::Point2f newPosition, bool isMeasurementValid = true) {
        if (isMeasurementValid) {
            measurement.at<float>(0) = newPosition.x;
            measurement.at<float>(1) = newPosition.y;
        } else {
            // Use prediction as measurement if the actual measurement is not valid
            cv::Mat prediction = KF.predict();
            measurement.at<float>(0) = prediction.at<float>(0);
            measurement.at<float>(1) = prediction.at<float>(1);
        }
        KF.correct(measurement);
    }

private:
    cv::KalmanFilter KF;
    cv::Mat measurement;
};

#endif // SIMPLEKALMANFILTER_H
