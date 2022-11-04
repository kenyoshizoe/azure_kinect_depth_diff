cmake_minimum_required(VERSION 3.1)
enable_language(CXX)
set(CMAKE_CXX_STANDARD 14) # C++14...
set(CMAKE_CXX_STANDARD_REQUIRED ON) #...is required...
set(CMAKE_CXX_EXTENSIONS OFF) #...without compiler extensions like gnu++11

find_package(k4a REQUIRED)
find_package(OpenCV REQUIRED)

project(azure_kinect_depth_diff)

add_executable( main
    main.cpp
)

target_include_directories( main
    PUBLIC
    ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries( main
    k4a::k4a
    ${OpenCV_LIBRARIES}
)
