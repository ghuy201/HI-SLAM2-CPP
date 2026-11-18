find_package(yaml-cpp CONFIG REQUIRED)

add_library(hislam_config STATIC
    src/config.cpp
)

add_library(hislam::config ALIAS hislam_config)

target_compile_features(hislam_config
    PUBLIC
        cxx_std_20
)

target_include_directories(hislam_config
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)


target_link_libraries(hislam_config
    PRIVATE
        yaml-cpp::yaml-cpp
)
set_target_properties(hislam_config PROPERTIES
    EXPORT_NAME config
)
