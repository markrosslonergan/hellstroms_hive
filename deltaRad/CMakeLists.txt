cmake_minimum_required (VERSION 3.3)
project (DeltaRad)
include(xgboost/cmake/Utils.cmake)

# The version number.
set (DeltaRad_VERSION_MAJOR 1)
set (DeltaRad_VERSION_MINOR 0)


if (CMAKE_VERSION VERSION_LESS "3.1")
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
          set (CMAKE_CXX_FLAGS "--std=c++11 ${CMAKE_CXX_FLAGS}")
    endif ()
    else ()
	  set (CMAKE_CXX_STANDARD 14)
endif ()

list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
#This works if your root as compiled with cmake, 
find_package(ROOT REQUIRED COMPONENTS TMVA TreePlayer)# COMPONENTS Core Cint RIO Net Hist Graf Graf3d Gpad Tree Rint Postscript Matrix Physics MathCore Thread)
include(${ROOT_USE_FILE})

set (ROOTXTRAS "-lTMVA -lTreePlayer")
execute_process(COMMAND "root-config" "--cflags" "--libs" OUTPUT_VARIABLE rootcflag)
execute_process(COMMAND "root-config" "--incdir" OUTPUT_VARIABLE ROOT_INCDIR)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  ${ROOTXTRAS} ")

#set (CMAKE_CXX_FLAGS "-std=c++1y ${CMAKE_CXX_FLAGS}")
message(STATUS "OUTPUT of CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")

include_directories("${PROJECT_BINARY_DIR}")
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc/)
include_directories(${ROOT_INCDIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/tinyxml/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/xgboost/include/)

add_subdirectory(tinyxml)
add_subdirectory(src)
add_subdirectory(xgboost)

#file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/xml/default.xml DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/build/src/)

