cmake_minimum_required (VERSION 2.6)
project (delta_study)
# The version number.

if (CMAKE_VERSION VERSION_LESS "3.1")
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
          set (CMAKE_CXX_FLAGS "--std=gnu++11 ${CMAKE_CXX_FLAGS}")
    endif ()
    else ()
	    set (CMAKE_CXX_STANDARD 11)
endif ()

#set (CMAKE_CXX_FLAGS "-L/grid/fermiapp/products/larsoft/root/v5_34_32/Linux64bit+2.6-2.12-e9-nu-prof/lib -lCore -lCint -lRIO -lNet -lHist -lGraf -lGraf3d -lGpad -lTree -lRint -lPostscript -lMatrix -lPhysics -lMathCore -lThread -pthread -lm -ldl -rdynamic -pthread -std=c++1y -Wno-deprecated-declarations -m64 -I/grid/fermiapp/products/larsoft/root/v5_34_32/Linux64bit+2.6-2.12-e9-nu-prof/include ")

list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})

find_package(ROOT REQUIRED)# COMPONENTS Core Cint RIO Net Hist Graf Graf3d Gpad Tree Rint Postscript Matrix Physics MathCore Thread)
include(${ROOT_USE_FILE})

## So these next two lines on duncan
find_package(GSL REQUIRED)
include_directories(${GSL_INCLUDE_DIRS})
#and this line for nevis
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lgsl -lgslcblas -lm -I/usr/include")



set (SBNLIB $ENV{SBNFITDIR})

include_directories("${PROJECT_BINARY_DIR}")
include_directories(${SBNLIB}/inc)
include_directories(${SBNLIB}/tinyxml)

link_directories(${SBNLIB}/build/tinyxml)
link_directories(${SBNLIB}/build/src)

add_subdirectory(src)





