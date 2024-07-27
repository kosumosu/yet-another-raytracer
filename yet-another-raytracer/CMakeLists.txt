cmake_minimum_required(VERSION 3.28)
project(yet_another_raytracer)

set(CMAKE_CXX_STANDARD 20)

include(CheckIPOSupported)
check_ipo_supported(RESULT supported OUTPUT error)

add_executable(yet_another_raytracer
        BitmapTexture.cpp
        BlinnMaterial.cpp
        CameraCommandFactory.cpp
        CameraCommandProcessor.cpp
        DielectricMaterial.cpp
        DirectionalLightSource.cpp
        Film.cpp
        FlatTriangleObject.cpp
        GeometryLightSource.cpp
        KDTreeAccelerator.cpp
        KDTreeMarcher.cpp
        KDTreeNode.cpp
        LightSourceCommandFactory.cpp
        LightSourceCommandProcessor.cpp
        MaterialCommandFactory.cpp
        MaterialCommandProcessor.cpp
        MonteCarloPathIntegrator.cpp
        NullAccelerator.cpp
        NullMarcher.cpp
        OSAbstraction.cpp
        ObjectCommandFactory.cpp
        ObjectCommandProcessor.cpp
        Parser.cpp
        PointLightSource.cpp
        ProcessTimeStopwatch.cpp
        Raytracer.cpp
        SceneLoader.cpp
        SettingsCommandFactory.cpp
        SettingsCommandProcessor.cpp
        SkyLightSource.cpp
        SphereObject.cpp
        StdHigheResolutionClockStopwatch.cpp
        TextureCommandFactory.cpp
        TextureCommandProcessor.cpp
        TransformCommandFactory.cpp
        TransformCommandProcessor.cpp
        main.cpp
)

target_include_directories(yet_another_raytracer PRIVATE ".")
include_directories("../../png++-0.2.9")

find_package(PNG REQUIRED)
include_directories(${PNG_INCLUDE_DIR})
target_link_libraries(yet_another_raytracer ${PNG_LIBRARY})

target_compile_options(yet_another_raytracer PRIVATE "-Wno-deprecated-enum-enum-conversion")
if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(yet_another_raytracer PRIVATE "-ffast-math" "-funroll-loops" "-O3" "-Wno-deprecated-enum-enum-conversion")
endif ()

if( supported )
    message(STATUS "IPO / LTO enabled")
    set_property(TARGET yet_another_raytracer PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
    message(STATUS "IPO / LTO not supported: <${error}>")
endif()
