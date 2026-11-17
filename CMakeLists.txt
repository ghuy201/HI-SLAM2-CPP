cmake_minimum_required(VERSION 3.24)
project(hi-slam2-cpp LANGUAGES CXX CUDA)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# CUDA C++20
set(CMAKE_CUDA_STANDARD 20)
set(CMAKE_CUDA_STANDARD_REQUIRED ON)

# RTX 5090: Blackwell, SM 12.0
set(CMAKE_CUDA_ARCHITECTURES 120)

# Export compile commands for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Eigen3 3.3 REQUIRED NO_MODULE)

add_library(hislam STATIC
    src/slam.cpp

    src/tracking/frontend.cpp
    src/tracking/backend.cpp
)

target_include_directories(hislam
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    PRIVATE
        ${PROJECT_SOURCE_DIR}/src
)

target_link_libraries(hislam
    PUBLIC
        Eigen3::Eigen
)

add_executable(main examples/main.cpp)
target_link_libraries(main
    PUBLIC
        hislam
)

