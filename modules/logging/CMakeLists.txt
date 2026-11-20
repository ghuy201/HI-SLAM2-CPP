add_library(hislam_logging INTERFACE)

add_library(hislam::logging ALIAS hislam_logging)

target_compile_features(hislam_logging
    INTERFACE
        cxx_std_20
)

target_include_directories(hislam_logging
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)


target_link_libraries(hislam_logging
    INTERFACE
        hislam::config
)
