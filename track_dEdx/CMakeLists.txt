cmake_minimum_required (VERSION 2.6)
project (DeltaRad)
# The version number.
set (DeltaRad_VERSION_MAJOR 1)
set (DeltaRad_VERSION_MINOR 0)

if (CMAKE_VERSION VERSION_LESS "3.1")
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
          set (CMAKE_CXX_FLAGS "--std=c++14 ${CMAKE_CXX_FLAGS}")
    endif ()
    else ()
	  set (CMAKE_CXX_STANDARD 14)
endif ()


find_package(ROOT REQUIRED)# COMPONENTS Core Cint RIO Net Hist Graf Graf3d Gpad Tree Rint Postscript Matrix Physics MathCore Thread)
include(${ROOT_USE_FILE})
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})


include_directories("${PROJECT_BINARY_DIR}")


add_executable (trkdEdx trk_dEdx.cxx TruncMean.cxx)
target_link_libraries (trkdEdx ${ROOT_LIBRARIES})


