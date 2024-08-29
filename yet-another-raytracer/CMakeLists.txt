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
        objects/FlatTriangleObject.cpp
        GeometryLightSource.cpp
        accelerators/kd_tree/KDTreeAccelerator.cpp
        accelerators/kd_tree/KDTreeMarcher.cpp
        accelerators/kd_tree/KDTreeNode.cpp
        LightSourceCommandFactory.cpp
        LightSourceCommandProcessor.cpp
        MaterialCommandFactory.cpp
        MaterialCommandProcessor.cpp
        accelerators/null/NullAccelerator.cpp
        accelerators/null/NullMarcher.cpp
        OSAbstraction.cpp
        ObjectCommandFactory.cpp
        ObjectCommandProcessor.cpp
        Parser.cpp
        PointLightSource.cpp
        ProcessTimeStopwatch.cpp
        SceneLoader.cpp
        SettingsCommandFactory.cpp
        SettingsCommandProcessor.cpp
        SkyLightSource.cpp
        objects/SphereObject.cpp
        StdHighResolutionClockStopwatch.cpp
        TextureCommandFactory.cpp
        TextureCommandProcessor.cpp
        TransformCommandFactory.cpp
        TransformCommandProcessor.cpp
        main.cpp
        applications/NanaApplication.cpp
)

target_include_directories(yet_another_raytracer PRIVATE ".")
include_directories(${PNGPP_INCLUDE_DIR})

#find_package(ZLIB REQUIRED)

#find_package(PNG REQUIRED)
#add_definitions(${PNG_DEFINITIONS})
#include_directories(${PNG_INCLUDE_DIR})
#target_link_libraries(yet_another_raytracer ${PNG_LIBRARIES})


include(FetchContent)

FetchContent_Declare(
        libpng
        GIT_REPOSITORY https://git.code.sf.net/p/libpng/code
        GIT_TAG        libpng16
)
set(PNG_TESTS OFF)

FetchContent_Declare(
        nana
        GIT_REPOSITORY https://github.com/cnjinhao/nana.git
        GIT_TAG        v1.7.4
)

FetchContent_GetProperties(libpng)
if(NOT libpng_POPULATED)
    FetchContent_Populate(libpng)
    FetchContent_MakeAvailable(libpng)
    add_subdirectory("${libpng_SOURCE_DIR}" ${libpng_BINARY_DIR} EXCLUDE_FROM_ALL)
    include_directories(${libpng_SOURCE_DIR})
endif()

FetchContent_GetProperties(nana)
if(NOT nana_POPULATED)
    FetchContent_Populate(nana)
    FetchContent_MakeAvailable(nana)
    add_subdirectory(${nana_SOURCE_DIR} ${nana_BINARY_DIR})
endif()


target_link_libraries(yet_another_raytracer PRIVATE
        png_static
        nana
)




target_compile_options(yet_another_raytracer PRIVATE "-Wno-deprecated-enum-enum-conversion")
target_link_options(yet_another_raytracer PRIVATE "-municode")
if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(yet_another_raytracer PRIVATE "-ffast-math" "-funroll-loops" "-O3" "-Wno-deprecated-enum-enum-conversion")
endif ()

include(CheckIPOSupported)
check_ipo_supported(RESULT supported OUTPUT error)
if( supported )
    message(STATUS "IPO / LTO enabled")
    set_property(TARGET yet_another_raytracer PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
    message(STATUS "IPO / LTO not supported: <${error}>")
endif()
