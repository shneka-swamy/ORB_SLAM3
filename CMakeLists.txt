cmake_minimum_required(VERSION 2.8)
project(ORB_SLAM3)

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Release)
ENDIF()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_BINARY_DIR}/conan/")
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_BINARY_DIR}/conan/")

MESSAGE("Build type: " ${CMAKE_BUILD_TYPE})

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  -Wall   -O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall   -O3")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -march=native")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -march=native")

# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wno-deprecated -O3 -march=native ")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-deprecated -O3 -march=native")

if (NOT EXISTS "${CMAKE_BINARY_DIR}/conan/")
  file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/conan/")
endif()
if (NOT EXISTS "${CMAKE_BINARY_DIR}/conan/conan.cmake")
  message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
  file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/master/conan.cmake"
                "${CMAKE_BINARY_DIR}/conan/conan.cmake")
endif()
include(${CMAKE_BINARY_DIR}/conan/conan.cmake)

conan_cmake_configure(
  REQUIRES
  #  opencv/4.5.5
  boost/1.72.0
  zlib/1.2.12
  GENERATORS
  cmake_find_package_multi
  )

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(conan_build "Debug")
else()
  set(conan_build "Release")
endif()

conan_cmake_autodetect(settings BUILD_TYPE ${conan_build})

conan_cmake_install(
  PATH_OR_REFERENCE .
  INSTALL_FOLDER ${CMAKE_BINARY_DIR}/conan
  BUILD missing
  REMOTE conancenter
  SETTINGS ${settings}
  )


# Check C++11 or C++0x support
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
   add_definitions(-DCOMPILEDWITHC11)
   message(STATUS "Using flag -std=c++11.")
elseif(COMPILER_SUPPORTS_CXX0X)
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
   add_definitions(-DCOMPILEDWITHC0X)
   message(STATUS "Using flag -std=c++0x.")
else()
   message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

LIST(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake_modules)

find_package(OpenCV 4.0 COMPONENTS core features2d highgui imgproc imgcodecs)
if(NOT OpenCV_FOUND)
  find_package(OpenCV 3.0)
  if(NOT OpenCV_FOUND)
    message(FATAL_ERROR "OpenCV > 3.0 not found.")
  endif()
endif()

MESSAGE("OPENCV VERSION:")
MESSAGE(${OpenCV_VERSION})

find_package(Eigen3 3.1.0 REQUIRED)
find_package(Pangolin REQUIRED)
find_package(realsense2)

set(OPENCV_LIBRARIES
	#opencv::opencv_calib2d
  opencv::opencv_core
  opencv::opencv_features2d
  opencv::opencv_highgui
  opencv::opencv_imgproc
  opencv::opencv_imgcodecs
  )

MESSAGE(STATUS "${OpenCV_INCLUDE_DIRS} ${OpenCV_LIBS}")

set(OPENCV_LIBRARIES
	${OpenCV_LIBS})

include_directories(
${PROJECT_SOURCE_DIR}
${PROJECT_SOURCE_DIR}/include
${PROJECT_SOURCE_DIR}/include/CameraModels
${EIGEN3_INCLUDE_DIR}
${Pangolin_INCLUDE_DIRS}
)

function(compile targetName cppFiles)

  add_executable(${targetName}
    src/System.cc
    src/Tracking.cc
    src/LocalMapping.cc
    src/LoopClosing.cc
    src/ORBextractor.cc
    src/ORBmatcher.cc
    src/FrameDrawer.cc
    src/Converter.cc
    src/MapPoint.cc
    src/KeyFrame.cc
    src/Atlas.cc
    src/Map.cc
    src/MapDrawer.cc
    src/Optimizer.cc
    src/Frame.cc
    src/KeyFrameDatabase.cc
    src/Sim3Solver.cc
    src/Initializer.cc
    src/Viewer.cc
    src/ImuTypes.cc
    src/G2oTypes.cc
    src/CameraModels/Pinhole.cpp
    src/CameraModels/KannalaBrandt8.cpp
    src/OptimizableTypes.cpp
    src/MLPnPsolver.cpp
    src/TwoViewReconstruction.cc
    include/System.h
    include/Tracking.h
    include/LocalMapping.h
    include/LoopClosing.h
    include/ORBextractor.h
    include/ORBmatcher.h
    include/FrameDrawer.h
    include/Converter.h
    include/MapPoint.h
    include/KeyFrame.h
    include/Atlas.h
    include/Map.h
    include/MapDrawer.h
    include/Optimizer.h
    include/Frame.h
    include/KeyFrameDatabase.h
    include/Sim3Solver.h
    include/Initializer.h
    include/Viewer.h
    include/ImuTypes.h
    include/G2oTypes.h
    include/CameraModels/GeometricCamera.h
    include/CameraModels/Pinhole.h
    include/CameraModels/KannalaBrandt8.h
    include/OptimizableTypes.h
    include/MLPnPsolver.h
    include/TwoViewReconstruction.h
    include/Config.h
    )

  target_include_directories(${targetName} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    )

  target_include_directories(${targetName} SYSTEM PUBLIC
    ${Pangolin_INCLUDE_DIRS}
    # ${OpenCV_INCLUDE_DIRS}
    )

  target_link_libraries(${targetName} PUBLIC
    ${OPENCV_LIBRARIES}
    Eigen3::Eigen
    ${Pangolin_LIBRARIES}
    DBoW2
    g2o
    )

  set_target_properties(${targetName}
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )

endfunction()

add_subdirectory(Thirdparty/DBoW2)
add_subdirectory(Thirdparty/g2o)

compile(rgbd_tum Examples/RGB-D/rgbd_tum.cc)

compile(stereo_kitti Examples/Stereo/stereo_kitti.cc)

compile(stereo_euroc Examples/Stereo/stereo_euroc.cc)

compile(stereo_tum_vi Examples/Stereo/stereo_tum_vi.cc)

compile(mono_tum Examples/Monocular/mono_tum.cc)

compile(mono_kitti Examples/Monocular/mono_kitti.cc)

compile(mono_euroc Examples/Monocular/mono_euroc.cc)

compile(mono_tum_vi Examples/Monocular/mono_tum_vi.cc)

compile(mono_inertial_euroc Examples/Monocular-Inertial/mono_inertial_euroc.cc)

compile(mono_inertial_tum_vi Examples/Monocular-Inertial/mono_inertial_tum_vi.cc)

compile(stereo_inertial_euroc Examples/Stereo-Inertial/stereo_inertial_euroc.cc)

compile(stereo_inertial_tum_vi Examples/Stereo-Inertial/stereo_inertial_tum_vi.cc)

# set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
#
# add_library(${PROJECT_NAME} SHARED
# src/System.cc
# src/Tracking.cc
# src/LocalMapping.cc
# src/LoopClosing.cc
# src/ORBextractor.cc
# src/ORBmatcher.cc
# src/FrameDrawer.cc
# src/Converter.cc
# src/MapPoint.cc
# src/KeyFrame.cc
# src/Atlas.cc
# src/Map.cc
# src/MapDrawer.cc
# src/Optimizer.cc
# src/Frame.cc
# src/KeyFrameDatabase.cc
# src/Sim3Solver.cc
# src/Initializer.cc
# src/Viewer.cc
# src/ImuTypes.cc
# src/G2oTypes.cc
# src/CameraModels/Pinhole.cpp
# src/CameraModels/KannalaBrandt8.cpp
# src/OptimizableTypes.cpp
# src/MLPnPsolver.cpp
# src/TwoViewReconstruction.cc
# include/System.h
# include/Tracking.h
# include/LocalMapping.h
# include/LoopClosing.h
# include/ORBextractor.h
# include/ORBmatcher.h
# include/FrameDrawer.h
# include/Converter.h
# include/MapPoint.h
# include/KeyFrame.h
# include/Atlas.h
# include/Map.h
# include/MapDrawer.h
# include/Optimizer.h
# include/Frame.h
# include/KeyFrameDatabase.h
# include/Sim3Solver.h
# include/Initializer.h
# include/Viewer.h
# include/ImuTypes.h
# include/G2oTypes.h
# include/CameraModels/GeometricCamera.h
# include/CameraModels/Pinhole.h
# include/CameraModels/KannalaBrandt8.h
# include/OptimizableTypes.h
# include/MLPnPsolver.h
# include/TwoViewReconstruction.h
# include/Config.h
# )
#
# add_subdirectory(Thirdparty/g2o)
#
# target_link_libraries(${PROJECT_NAME}
# ${OpenCV_LIBS}
# ${EIGEN3_LIBS}
# ${Pangolin_LIBRARIES}
# ${PROJECT_SOURCE_DIR}/Thirdparty/DBoW2/lib/libDBoW2.so
# ${PROJECT_SOURCE_DIR}/Thirdparty/g2o/lib/libg2o.so
# -lboost_serialization
# -lcrypto
# )
#
#
# ### Build examples
#
# # RGB-D examples
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/Examples/RGB-D)
#
# add_executable(rgbd_tum Examples/RGB-D/rgbd_tum.cc)
# target_link_libraries(rgbd_tum ${PROJECT_NAME})
#
#
# # Stereo examples
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/Examples/Stereo)
#
# add_executable(stereo_kitti Examples/Stereo/stereo_kitti.cc)
# target_link_libraries(stereo_kitti ${PROJECT_NAME})
#
# add_executable(stereo_euroc Examples/Stereo/stereo_euroc.cc)
# target_link_libraries(stereo_euroc ${PROJECT_NAME})
#
# add_executable(stereo_tum_vi Examples/Stereo/stereo_tum_vi.cc)
# target_link_libraries(stereo_tum_vi ${PROJECT_NAME})
#
# # Monocular examples
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/Examples/Monocular)
#
# add_executable(mono_tum Examples/Monocular/mono_tum.cc)
# target_link_libraries(mono_tum ${PROJECT_NAME})
#
# add_executable(mono_kitti Examples/Monocular/mono_kitti.cc)
# target_link_libraries(mono_kitti ${PROJECT_NAME})
#
# add_executable(mono_euroc Examples/Monocular/mono_euroc.cc)
# target_link_libraries(mono_euroc ${PROJECT_NAME})
#
# add_executable(mono_tum_vi Examples/Monocular/mono_tum_vi.cc)
# target_link_libraries(mono_tum_vi ${PROJECT_NAME})
#
# # Monocular-Inertial examples
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/Examples/Monocular-Inertial)
#
# add_executable(mono_inertial_euroc Examples/Monocular-Inertial/mono_inertial_euroc.cc)
# target_link_libraries(mono_inertial_euroc ${PROJECT_NAME})
#
# add_executable(mono_inertial_tum_vi Examples/Monocular-Inertial/mono_inertial_tum_vi.cc)
# target_link_libraries(mono_inertial_tum_vi ${PROJECT_NAME})
#
# # Stereo-Inertial examples
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/Examples/Stereo-Inertial)
#
# add_executable(stereo_inertial_euroc Examples/Stereo-Inertial/stereo_inertial_euroc.cc)
# target_link_libraries(stereo_inertial_euroc ${PROJECT_NAME})
#
# add_executable(stereo_inertial_tum_vi Examples/Stereo-Inertial/stereo_inertial_tum_vi.cc)
# target_link_libraries(stereo_inertial_tum_vi ${PROJECT_NAME})

