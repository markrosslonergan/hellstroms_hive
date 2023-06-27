cmake_minimum_required (VERSION 3.14)
project (Hive VERSION 2.0.0 )
include(xgboost/cmake/Utils.cmake)
include(FetchContent)

# Fetch necessary dependency --> require cmake version >= 3.14
FetchContent_Declare(
  sbnfit
  GIT_REPOSITORY https://github.com/NevisUB/whipping_star.git 
  GIT_TAG        7eabaf401154cf1011d7a472dcd7fae59ee5c781 # <- New master w/memory issues tweaks   | 946cab5487dfdb2542e57a46e32c39000aa84927  #641861404b092fa3113ada53f93385adc03936d0     #  branch guanqun_fractionfit
)

# After the following call, the CMake targets defined by sbnfit 
# will be available to the rest of the build
FetchContent_MakeAvailable(sbnfit)


# The version number.
set (Hive_VERSION_MAJOR 2)
set (Hive_VERSION_MINOR 0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (CMAKE_VERSION VERSION_LESS "3.1")
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
          set (CMAKE_CXX_FLAGS "--std=c++17 ${CMAKE_CXX_FLAGS}")
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

# Update compile flags in order to be able to compile sbnfit related files
# Get our GSL libraries
FIND_PROGRAM(GSL_CONFIG gsl-config)
EXEC_PROGRAM(${GSL_CONFIG} ARGS --cflags OUTPUT_VARIABLE GSL_CXX_FLAGS)
EXEC_PROGRAM(${GSL_CONFIG} ARGS --libs OUTPUT_VARIABLE GSL_LIBS)

set(EIGEN_CONFIG "$ENV{EIGEN_INC}")

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GSL_LIBS} ${GSL_CXX_FLAGS} -g -I ${EIGEN_CONFIG} -w -Wno-deprecated-declarations")

#set (CMAKE_CXX_FLAGS "-std=c++1y ${CMAKE_CXX_FLAGS}")
message(STATUS "OUTPUT of CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")

include_directories("${PROJECT_BINARY_DIR}")
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc/)
include_directories(${ROOT_INCDIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/tinyxml/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/xgboost/include/)

add_subdirectory(root_linkdefs)
add_subdirectory(tinyxml)
add_subdirectory(src)
add_subdirectory(xgboost)

#file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/xml/default.xml DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/build/src/)

