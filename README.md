
# Multi-Camera Setup

Welcome to the Multi-Camera Setup project! This repository contains code for setting up and utilizing multiple cameras for real-time tracking and 3D position estimation of a moving object.

## Table of Contents

- [Introduction](#introduction)
- [Features](#features)
- [Installation](#installation)
- [Usage](#usage)
- [Project Structure](#project-structure)
- [Additional Notes](#additional-notes)
- [Contributing](#contributing)
- [License](#license)

## Introduction

This project demonstrates a multi-camera system for tracking a moving object (e.g., a ball) in real-time using OpenCV and C++. It includes camera calibration (given), video processing, and 3D point triangulation.

## Features

- Camera calibration and parameter loading from JSON files.
- Real-time video processing with OpenCV.
- Multi-threaded frame processing for efficiency.
- Object tracking and speed calculation.
- 3D position estimation using triangulation.

## Installation

To get started, clone this repository and install the required dependencies.

\`\`\`bash
git clone https://github.com/barakooda/cv_learn.git
cd cv_learn
\`\`\`

Ensure you have the following dependencies installed:

- OpenCV
- nlohmann/json

## Usage

1. **Configure Camera Parameters**: Place your camera parameter JSON files in the \`calibration\` folder.
2. **Place Videos**: Place your video files in the \`videos\` folder.
3. **Build the Project**: Use CMake to build the project.
   
   \`\`\`bash
   mkdir build
   cd build
   cmake ..
   make
   \`\`\`
4. **Run the Program**: Execute the compiled program.
   
   \`\`\`bash
   ./cv_learn
   \`\`\`

## Project Structure

\`\`\`
cv_learn/
├── calibration/      # Camera calibration files
├── csv_files/        # Output CSV files
├── l2graph/          # Additional resources
├── src/              # Source code
├── videos/           # Video files
├── .gitignore
├── LICENSE
└── README.md
\`\`\`

## Additional Notes

This project is still a work in progress. Future improvements and learning objectives include:

- Implementing Optical Flow.
- Kalman Filter.
- Combining detection and Optical Flow outputs in Kalmain filter.



