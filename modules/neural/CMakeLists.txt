find_package(Eigen3 3.3 REQUIRED NO_MODULE)
find_package(OpenCV REQUIRED)
find_package(Torch REQUIRED)
find_package(CUDAToolkit REQUIRED)

add_library(hislam_neural STATIC
    src/droidnet.cpp
)

add_library(hislam::neural ALIAS hislam_neural)

target_compile_features(hislam_neural
    PUBLIC
        cxx_std_20
        cuda_std_20
)

target_include_directories(hislam_neural
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${OpenCV_INCLUDE_DIRS}

)


target_link_libraries(hislam_neural
    PRIVATE
        hislam::config
        Eigen3::Eigen
        ${OpenCV_LIBS}
        ${TORCH_LIBRARIES}
)
set_target_properties(hislam_neural PROPERTIES
    EXPORT_NAME neural
)
