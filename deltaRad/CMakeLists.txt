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


#This works if your root as compiled with cmake, 
find_package(ROOT REQUIRED TMVA TreePlayer)# COMPONENTS Core Cint RIO Net Hist Graf Graf3d Gpad Tree Rint Postscript Matrix Physics MathCore Thread)
include(${ROOT_USE_FILE})


list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})



#set (ROOTXTRAS "-lTMVA -lTreePlayer")
#execute_process(COMMAND "root-config" "--cflags" "--libs" OUTPUT_VARIABLE rootcflag)
#execute_process(COMMAND "root-config" "--libdir" OUTPUT_VARIABLE ROOT_LIBDIR)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ROOTXTRAS} ${rootcflag}")


message(STATUS "OUTPUT of CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")


include_directories("${PROJECT_BINARY_DIR}")
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/tinyxml/)

add_subdirectory(tinyxml)
add_subdirectory(src)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/xml/default.xml DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/build/src/)

