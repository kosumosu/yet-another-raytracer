cmake_minimum_required(VERSION 3.28)
project(yet_another_raytracer)

set(CMAKE_CXX_STANDARD 20)

add_executable(yet_another_raytracer
        BitmapTexture.cpp
        materials/BlinnMaterial.cpp
        CameraCommandFactory.cpp
        CameraCommandProcessor.cpp
        materials/DielectricMaterial.cpp
        lights/DirectionalLightSource.cpp
        Film.cpp
        objects/FlatTriangleObject.cpp
        lights/GeometryLightSource.cpp
        accelerators/kd_tree/KDTreeAccelerator.cpp
        accelerators/kd_tree/KDTreeMarcher.cpp
        accelerators/kd_tree/KDTreeNode.cpp
        LightSourceCommandFactory.cpp
        LightSourceCommandProcessor.cpp
        MaterialCommandFactory.cpp
        MaterialCommandProcessor.cpp
        OSAbstraction.cpp
        ObjectCommandFactory.cpp
        ObjectCommandProcessor.cpp
        Parser.cpp
        lights/PointLightSource.cpp
        ProcessTimeStopwatch.cpp
        SceneLoader.cpp
        SettingsCommandFactory.cpp
        SettingsCommandProcessor.cpp
        lights/SkyLightSource.cpp
        objects/SphereObject.cpp
        StdHighResolutionClockStopwatch.cpp
        TextureCommandFactory.cpp
        TextureCommandProcessor.cpp
        TransformCommandFactory.cpp
        TransformCommandProcessor.cpp
        main.cpp
        applications/NanaApplication.cpp
        debug_printing.cpp
)

target_include_directories(yet_another_raytracer PRIVATE ".")
include_directories(${PNGPP_INCLUDE_DIR})
include_directories(../3rdparty)

# Works on windows via vcpkg with
# libpng x64-mingw-static (MinGW) OR x64-windows (VC)
find_package(ZLIB REQUIRED)
find_package(PNG REQUIRED)
target_link_libraries(yet_another_raytracer PRIVATE PNG::PNG)

find_package(OpenEXR CONFIG REQUIRED)
target_link_libraries(yet_another_raytracer PRIVATE OpenEXR::OpenEXR)

IF (NOT DEFINED ENABLE_UI)
    SET(ENABLE_UI true)
ENDIF()

if (ENABLE_UI)
    ADD_DEFINITIONS(-DENABLE_UI_DEF)

    include(FetchContent)

    FetchContent_Declare(
            nana
            GIT_REPOSITORY https://github.com/kosumosu/nana.git
            GIT_TAG develop-1.8
    )

    FetchContent_GetProperties(nana)
    if (NOT nana_POPULATED)
        FetchContent_MakeAvailable(nana)
    endif ()

    target_link_libraries(yet_another_raytracer PRIVATE
            nana
    )
endif ()

if (MINGW)
    target_link_options(yet_another_raytracer PRIVATE "-municode")
endif ()

if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    if (NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(yet_another_raytracer PRIVATE "-funroll-loops" "-O3" "-Wno-deprecated-enum-enum-conversion")
    endif ()
endif ()

if (NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    include(CheckIPOSupported)
    check_ipo_supported(RESULT supported OUTPUT error)
    if (supported)
        message(STATUS "IPO / LTO enabled")
        set_property(TARGET yet_another_raytracer PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
    else ()
        message(STATUS "IPO / LTO not supported: <${error}>")
    endif ()
endif ()

