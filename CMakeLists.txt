cmake_minimum_required(VERSION 3.10)

project(multi_camera_setup)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Enable compiler optimizations
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /GL /Oi /Ot /Ob2 /Gy")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /LTCG /OPT:REF /OPT:ICF")

# Specify the path to your OpenCV build directory
set(OpenCV_DIR "D:/opencv/build")

find_package(OpenCV REQUIRED)

# Include directories
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(D:/opencv/source/opencv/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Source files
file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)

# Executable
add_executable(multi_camera_setup ${SOURCES})

# Link libraries
target_link_libraries(multi_camera_setup ${OpenCV_LIBS})

# Enable testing
enable_testing()
add_subdirectory(tests)


# Define a preprocessor macro with the project name
add_compile_definitions(PROJECT_NAME="${PROJECT_NAME}")