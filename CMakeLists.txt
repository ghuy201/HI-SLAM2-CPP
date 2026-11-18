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
set(TORCH_CUDA_ARCH_LIST "12.0")

# Export compile commands for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Setup CCACHE
find_program(CCACHE_PROGRAM ccache)

if(CCACHE_PROGRAM)
    message(STATUS "Using ccache: ${CCACHE_PROGRAM}")

    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
    set(CMAKE_CUDA_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
endif()

find_package(Eigen3 3.3 REQUIRED NO_MODULE)
find_package(OpenCV REQUIRED)
find_package(Torch REQUIRED)
find_package(CUDAToolkit REQUIRED)
find_package(GTSAM REQUIRED)
find_package(yaml-cpp CONFIG REQUIRED)

add_subdirectory(modules/config)
add_subdirectory(modules/tracking)

add_library(hislam STATIC
    src/slam.cpp
)

target_include_directories(hislam
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        ${OpenCV_INCLUDE_DIRS}
    PRIVATE
        ${PROJECT_SOURCE_DIR}/src
        CUDA::cudart
)

target_link_libraries(hislam
    PUBLIC
        Eigen3::Eigen
        hislam::config
        gtsam
        ${OpenCV_LIBS}
        ${TORCH_LIBRARIES}
    PRIVATE
        hislam::tracking

)

add_executable(main examples/main.cpp)
target_link_libraries(main
    PUBLIC
        hislam
)

