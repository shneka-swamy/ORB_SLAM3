cmake_minimum_required(VERSION 3.12.1)
project(ORB_SLAM3)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenCV 4.4 REQUIRED)
find_package(Eigen3 3.1.0 REQUIRED)
find_package(Pangolin REQUIRED)
# find_package(realsense2 QUIET)

add_subdirectory(Thirdparty/g2o)
add_subdirectory(Thirdparty/DBoW2)
add_subdirectory(Thirdparty/Sophus)

add_library(ORB_SLAM3
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
  src/Viewer.cc
  src/ImuTypes.cc
  src/G2oTypes.cc
  src/CameraModels/Pinhole.cpp
  src/CameraModels/KannalaBrandt8.cpp
  src/OptimizableTypes.cpp
  src/MLPnPsolver.cpp
  src/GeometricTools.cc
  src/TwoViewReconstruction.cc
  src/Config.cc
  src/Settings.cc
  )

set_target_properties(ORB_SLAM3 PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED YES
  CXX_EXTENSIONS NO
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
  )

target_include_directories(ORB_SLAM3 PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/include/CameraModels
  )

target_include_directories(ORB_SLAM3 SYSTEM PUBLIC
  ${EIGEN3_INLCUDE_DIR}
  ${Pangolin_INCLUDE_DIRS}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/Thirdparty/Sophus
  )

target_link_libraries(ORB_SLAM3 PUBLIC
  ${OpenCV_LIBS}
  ${EIGEN3_LIBS}
  ${Pangolin_LIBRARIES}
  DBoW2
  g2o
  -lboost_serialization # TODO: why is this needed?
  -lcrypto              # TODO: why is this needed?
  )

target_compile_options(ORB_SLAM3 PUBLIC
  -Wno-deprecated-declarations
  )

if (real_sense2_FOUND)
  target_include_directories(ORB_SLAM3 SYSTEM PUBLIC
    ${realsense_INCLUDE_DIR}
    )
  target_link_libraries(ORB_SLAM3 PUBLIC
    ${realsense2_LIBRARY}
    )
endif()

function(compileORB3 exec files)
  add_executable(${exec} ${files})
  target_link_libraries(${exec} ORB_SLAM3)
  set_target_properties(${exec} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    )
  target_compile_definitions(${exec} PRIVATE
    -DCOMPILEDWITHC11
    )
endfunction()

compileORB3(rgbd_tum Examples/RGB-D/rgbd_tum.cc)
compileORB3(stereo_kitti Examples/Stereo/stereo_kitti.cc)
compileORB3(stereo_euroc Examples/Stereo/stereo_euroc.cc)
compileORB3(stereo_tum_vi Examples/Stereo/stereo_tum_vi.cc)
compileORB3(mono_tum Examples/Monocular/mono_tum.cc)
compileORB3(mono_kitti Examples/Monocular/mono_kitti.cc)
compileORB3(mono_euroc Examples/Monocular/mono_euroc.cc)
compileORB3(mono_tum_vi Examples/Monocular/mono_tum_vi.cc)
compileORB3(mono_inertial_euroc Examples/Monocular-Inertial/mono_inertial_euroc.cc)
compileORB3(mono_inertial_tum_vi Examples/Monocular-Inertial/mono_inertial_tum_vi.cc)
compileORB3(stereo_inertial_euroc Examples/Stereo-Inertial/stereo_inertial_euroc.cc)
compileORB3(stereo_inertial_tum_vi Examples/Stereo-Inertial/stereo_inertial_tum_vi.cc)

if(realsense2_FOUND)
  compileORB3(rgbd_realsense_D435i Examples/RGB-D/rgbd_realsense_D435i.cc)
  compileORB3(rgb_inertial_realsense_D435i Examples/RGB-D-Inertial/rgbd_inertial_realsense_D435i.cc)
  compileORB3(stereo_realsense_t265 Examples/Stereo/stereo_realsense_t265.cc)
  compileORB3(stereo_realsense_D435i Examples/Stereo/stereo_realsense_D435i.cc)
  compileORB3(mono_realsense_t265 Examples/Monocular/mono_realsense_t265.cc)
  compileORB3(mono_realsense_D435i Examples/Monocular/mono_realsense_D435i.cc)
  compileORB3(mono_inertial_realsense_t265 Examples/Monocular-Inertial/mono_inertial_realsense_t265.cc)
  compileORB3(mono_inertial_realsense_D435i Examples/Monocular-Inertial/mono_inertial_realsense_D435i.cc)
  compileORB3(stereo_inertial_realsense_t265 Examples/Stereo-Inertial/stereo_inertial_realsense_t265.cc)
  compileORB3(stereo_inertial_realsense_D435i Examples/Stereo-Inertial/stereo_inertial_realsense_D435i.cc)
  compileORB3(recorder_realsense_D435i Examples/Calibration/recorder_realsense_D435i.cc)
  compileORB3(recorder_realsense_T265 Examples/Calibration/recorder_realsense_T265.cc)
  compileORB3(rgbd_realsense_T265 Examples/RGB-D/rgbd_realsense_T265.cc)
  compileORB3(rgbd_inertial_realsense_T265 Examples/RGB-D-Inertial/rgbd_inertial_realsense_T265.cc)
endif()

# Vocabulary/ORBvoc.txt not found then extract Vocabulary/ORBvoc.txt.tar.gz
if (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/Vocabulary/ORBvoc.txt)
  message(STATUS "Extracting ORBvoc.txt.tar.gz")
  execute_process(
    COMMAND tar -xf ${CMAKE_CURRENT_SOURCE_DIR}/Vocabulary/ORBvoc.txt.tar.gz -C ${CMAKE_CURRENT_SOURCE_DIR}/Vocabulary
    )
endif()

