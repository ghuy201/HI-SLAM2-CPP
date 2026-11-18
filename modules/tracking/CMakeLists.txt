find_package(Eigen3 3.3 REQUIRED NO_MODULE)
find_package(OpenCV REQUIRED)
find_package(Torch REQUIRED)
find_package(CUDAToolkit REQUIRED)
find_package(GTSAM REQUIRED)

add_library(hislam_tracking STATIC
    src/frontend.cpp
    src/backend.cpp
    src/motion_filter.cpp
)

add_library(hislam::tracking ALIAS hislam_tracking)

target_compile_features(hislam_tracking
    PUBLIC
        cxx_std_20
        cuda_std_20
)

target_include_directories(hislam_tracking
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${OpenCV_INCLUDE_DIRS}

)


target_link_libraries(hislam_tracking
    PRIVATE
        hislam::config
        Eigen3::Eigen
        gtsam
        ${OpenCV_LIBS}
        ${TORCH_LIBRARIES}
)
set_target_properties(hislam_tracking PROPERTIES
    EXPORT_NAME tracking
)
