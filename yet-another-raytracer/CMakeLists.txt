cmake_minimum_required(VERSION 3.28)
project(yet_another_raytracer)

set(CMAKE_CXX_STANDARD 20)

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
        applications/IApplication.h
        applications/ConsoleApplication.h
        SceneGeneration.h
        applications/NanaApplication.h
        applications/NanaApplication.cpp
)

target_include_directories(yet_another_raytracer PRIVATE ".")
include_directories(${PNGPP_INCLUDE_DIR})

#find_package(ZLIB REQUIRED)

find_package(PNG REQUIRED)
add_definitions(${PNG_DEFINITIONS})
include_directories(${PNG_INCLUDE_DIR})
target_link_libraries(yet_another_raytracer ${PNG_LIBRARIES})


include(FetchContent)


FetchContent_Declare(
        nana
        GIT_REPOSITORY https://github.com/cnjinhao/nana.git
        GIT_TAG        v1.7.4
)

FetchContent_GetProperties(nana)
if(NOT nana_POPULATED)
    FetchContent_Populate(nana)
    add_subdirectory(${nana_SOURCE_DIR} ${nana_BINARY_DIR})
endif()


target_link_libraries(yet_another_raytracer
        nana
)




target_compile_options(yet_another_raytracer PRIVATE "-Wno-deprecated-enum-enum-conversion")
target_link_options(yet_another_raytracer PRIVATE "-municode")
if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(yet_another_raytracer PRIVATE "-ffast-math" "-funroll-loops" "-O3" "-Wno-deprecated-enum-enum-conversion")
endif ()

#include(CheckIPOSupported)
#check_ipo_supported(RESULT supported OUTPUT error)
#if( supported )
#    message(STATUS "IPO / LTO enabled")
#    set_property(TARGET yet_another_raytracer PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
#else()
#    message(STATUS "IPO / LTO not supported: <${error}>")
#endif()
