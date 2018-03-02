# The extension is already found. Any number of sources could be listed here.
add_library (GeoAlgolib GeoAABox.cxx         GeoCone.cxx          GeoDirectedLine.cxx  GeoLine.cxx          GeoObjCollection.cxx GeoTrajectory.cxx
GeoAlgo.cxx          GeoCylinder.cxx      GeoHalfLine.cxx      GeoLineSegment.cxx   GeoSphere.cxx        GeoVector.cxx)

# Make sure the compiler can find include files for our Hello library
# when other libraries or executables link to Hello
#target_include_directories (tinyxmllib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
include_directories ( ${CMAKE_CURRENT_SOURCE_DIR})
