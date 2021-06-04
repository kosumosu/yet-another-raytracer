# Created by and for Qt Creator This file was created for editing the project sources only.
# You may attempt to use it for building too, by modifying this file here.

#TARGET = yet-another-raytracer

HEADERS = \
   $$PWD/Accelerator.h \
   $$PWD/Attenuation.h \
   $$PWD/base_matrix.hpp \
   $$PWD/base_matrix4x4.hpp \
   $$PWD/base_matrix4x4_transfrom.hpp \
   $$PWD/base_matrix_functions.hpp \
   $$PWD/base_matrix_operations.hpp \
   $$PWD/BitmapTexture.h \
   $$PWD/BlinnMaterial.h \
   $$PWD/bounding_box.hpp \
   $$PWD/bounding_box_functions.hpp \
   $$PWD/bounding_box_hit.hpp \
   $$PWD/BucketRenderer.h \
   $$PWD/Camera.h \
   $$PWD/CameraCommandFactory.h \
   $$PWD/CameraCommandProcessor.h \
   $$PWD/color_functions.hpp \
   $$PWD/CommandFactory.h \
   $$PWD/CommandProcessor.h \
   $$PWD/constants.hpp \
   $$PWD/DielectricMaterial.h \
   $$PWD/DirectionalLightSource.h \
   $$PWD/discrete_distribution.hpp \
   $$PWD/Distribution.h \
   $$PWD/Film.h \
   $$PWD/FlatTriangleObject.h \
   $$PWD/GeometryLightSource.h \
   $$PWD/GeometryObject.h \
   $$PWD/helper.hpp \
   $$PWD/Hit.h \
   $$PWD/IBucketSequencer.h \
   $$PWD/IExecutor.h \
   $$PWD/IRenderer.h \
   $$PWD/KDTraverseFrame.h \
   $$PWD/KDTreeAccelerator.h \
   $$PWD/KDTreeMarcher.h \
   $$PWD/KDTreeNode.h \
   $$PWD/LightingContext.h \
   $$PWD/LightSource.h \
   $$PWD/LightSourceCommandFactory.h \
   $$PWD/LightSourceCommandProcessor.h \
   $$PWD/LoadingContext.h \
   $$PWD/Marcher.h \
   $$PWD/Material.h \
   $$PWD/MaterialCommandFactory.h \
   $$PWD/MaterialCommandProcessor.h \
   $$PWD/MonteCarloPathIntegrator.h \
   $$PWD/new_math.hpp \
   $$PWD/NullAccelerator.h \
   $$PWD/NullMarcher.h \
   $$PWD/ObjectCommandFactory.h \
   $$PWD/ObjectCommandProcessor.h \
   $$PWD/OSAbstraction.h \
   $$PWD/Parser.h \
   $$PWD/ParserHelper.h \
   $$PWD/PointLightSource.h \
   $$PWD/ProcessTimeStopwatch.h \
   $$PWD/random.hpp \
   $$PWD/random_sample.hpp \
   $$PWD/ray.hpp \
   $$PWD/ray_functions.hpp \
   $$PWD/RayIntegrator.h \
   $$PWD/Raytracer.h \
   $$PWD/Scene.h \
   $$PWD/SceneLoader.h \
   $$PWD/SettingsCommandFactory.h \
   $$PWD/SettingsCommandProcessor.h \
   $$PWD/SingleThreadedScanlineRenderer.h \
   $$PWD/SkyLightSource.h \
   $$PWD/SolidColorTexture.h \
   $$PWD/SphereObject.h \
   $$PWD/sse_float_vector3.hpp \
   $$PWD/sse_float_vector3_operations.hpp \
   $$PWD/sse_float_vector4.hpp \
   $$PWD/sse_float_vector4_operations.hpp \
   $$PWD/StdHigheResolutionClockStopwatch.h \
   $$PWD/Stopwatch.h \
   $$PWD/targetver.h \
   $$PWD/Texture.h \
   $$PWD/TextureCommandFactory.h \
   $$PWD/TextureCommandProcessor.h \
   $$PWD/TextureCoords.h \
   $$PWD/ThreadBarrier.hpp \
   $$PWD/TopDownSequencer.h \
   $$PWD/transform_iterator.h \
   $$PWD/TransformableGeometryObject.h \
   $$PWD/TransformCommandFactory.h \
   $$PWD/TransformCommandProcessor.h \
   $$PWD/TransformStack.h \
   $$PWD/Types.h \
   $$PWD/UniformRandomBitGenerator.h \
   $$PWD/vector.hpp \
   $$PWD/vector3.hpp \
   $$PWD/vector3_operations.hpp \
   $$PWD/vector_functions.hpp \
   $$PWD/vector_operations.hpp \
   $$PWD/VisualizeUVTexture.h

SOURCES = \
   $$PWD/BitmapTexture.cpp \
   $$PWD/BlinnMaterial.cpp \
   $$PWD/BucketRenderer.cpp \
   $$PWD/CameraCommandFactory.cpp \
   $$PWD/CameraCommandProcessor.cpp \
   $$PWD/DielectricMaterial.cpp \
   $$PWD/DirectionalLightSource.cpp \
   $$PWD/Film.cpp \
   $$PWD/FlatTriangleObject.cpp \
   $$PWD/GeometryLightSource.cpp \
   $$PWD/KDTreeAccelerator.cpp \
   $$PWD/KDTreeMarcher.cpp \
   $$PWD/KDTreeNode.cpp \
   $$PWD/LightSourceCommandFactory.cpp \
   $$PWD/LightSourceCommandProcessor.cpp \
   $$PWD/main.cpp \
   $$PWD/MaterialCommandFactory.cpp \
   $$PWD/MaterialCommandProcessor.cpp \
   $$PWD/MonteCarloPathIntegrator.cpp \
   $$PWD/NullAccelerator.cpp \
   $$PWD/NullMarcher.cpp \
   $$PWD/ObjectCommandFactory.cpp \
   $$PWD/ObjectCommandProcessor.cpp \
   $$PWD/OSAbstraction.cpp \
   $$PWD/Parser.cpp \
   $$PWD/PointLightSource.cpp \
   $$PWD/ProcessTimeStopwatch.cpp \
   $$PWD/Raytracer.cpp \
   $$PWD/SceneLoader.cpp \
   $$PWD/SettingsCommandFactory.cpp \
   $$PWD/SettingsCommandProcessor.cpp \
   $$PWD/SingleThreadedScanlineRenderer.cpp \
   $$PWD/SkyLightSource.cpp \
   $$PWD/SphereObject.cpp \
   $$PWD/StdHigheResolutionClockStopwatch.cpp \
   $$PWD/TextureCommandFactory.cpp \
   $$PWD/TextureCommandProcessor.cpp \
   $$PWD/TransformCommandFactory.cpp \
   $$PWD/TransformCommandProcessor.cpp

INCLUDEPATH = \
    $$PWD/.

INCLUDEPATH += /usr/include/png++/


CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt
#DEFINES = 

unix|win32: LIBS += -lpng

unix:!macx: LIBS += -lpthread
