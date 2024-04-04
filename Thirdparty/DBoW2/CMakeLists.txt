cmake_minimum_required(VERSION 3.12.1)
project(DBoW2)

find_package(OpenCV 4 QUIET)
if(NOT OpenCV_FOUND)
   find_package(OpenCV 3.0 QUIET)
   if(NOT OpenCV_FOUND)
      message(FATAL_ERROR "OpenCV > 3.0 not found.")
   endif()
endif()

add_library(o_DBoW2
  DBoW2/BowVector.cpp
  DBoW2/FORB.cpp
  DBoW2/FeatureVector.cpp
  DBoW2/ScoringObject.cpp
  DUtils/Random.cpp
  DUtils/Timestamp.cpp)
target_link_libraries(o_DBoW2 ${OpenCV_LIBS})
set_target_properties(o_DBoW2 PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin"
  LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/lib"
  ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/lib"
)
target_compile_options(o_DBoW2 PUBLIC
  -Wno-deprecated-declarations
  )

# set(HDRS_DBOW2
#   DBoW2/BowVector.h
#   DBoW2/FORB.h 
#   DBoW2/FClass.h       
#   DBoW2/FeatureVector.h
#   DBoW2/ScoringObject.h   
#   DBoW2/TemplatedVocabulary.h)
# set(SRCS_DBOW2
#   DBoW2/BowVector.cpp
#   DBoW2/FORB.cpp      
#   DBoW2/FeatureVector.cpp
#   DBoW2/ScoringObject.cpp)
#
# set(HDRS_DUTILS
#   DUtils/Random.h
#   DUtils/Timestamp.h)
# set(SRCS_DUTILS
#   DUtils/Random.cpp
#   DUtils/Timestamp.cpp)
#
#
# find_package(OpenCV 4 QUIET)
# if(NOT OpenCV_FOUND)
#    find_package(OpenCV 3.0 QUIET)
#    if(NOT OpenCV_FOUND)
#       message(FATAL_ERROR "OpenCV > 3.0 not found.")
#    endif()
# endif()
#
# set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
#
#
#
# include_directories(${OpenCV_INCLUDE_DIRS})
# add_library(DBoW2 # SHARED
#   ${SRCS_DBOW2} ${SRCS_DUTILS})
# target_link_libraries(DBoW2 ${OpenCV_LIBS})

