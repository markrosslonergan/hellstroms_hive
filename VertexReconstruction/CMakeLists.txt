cmake_minimum_required (VERSION 2.8)
project (VertexReconstruction)
# The version number.
set (VertexReconstruction_VERSION_MAJOR 1)
set (VertexReconstruction_VERSION_MINOR 0)

if (CMAKE_VERSION VERSION_LESS "3.1")
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
          set (CMAKE_CXX_FLAGS "--std=c++14 ${CMAKE_CXX_FLAGS}")
    endif ()
    else ()
	  set (CMAKE_CXX_STANDARD 14)
endif ()


list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})

#This works if your root as compiled with cmake, 
find_package(ROOT REQUIRED COMPONENTS RIO Net TreePlayer)# COMPONENTS Core Cint RIO Net Hist Graf Graf3d Gpad Tree Rint Postscript Matrix Physics MathCore Thread)
include(${ROOT_USE_FILE})


#set (ROOTXTRAS "-lTMVA -lTreePlayer")
#execute_process(COMMAND "root-config" "--cflags" "--libs" OUTPUT_VARIABLE rootcflag)
#execute_process(COMMAND "root-config" "--libdir" OUTPUT_VARIABLE ROOT_LIBDIR)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ROOTXTRAS} ${rootcflag}")


message(STATUS "OUTPUT of CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")




include_directories("${PROJECT_BINARY_DIR}")
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/GeoAlgo/)


add_subdirectory(libio)

add_subdirectory(src)
add_subdirectory(GeoAlgo)
