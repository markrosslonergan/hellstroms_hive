include_directories (${CMAKE_SOURCE_DIR}/inc/)
include_directories ( ${CMAKE_SOURCE_DIR}/tinyxml/)


#add_executable (gLEE gLEE.cxx)
#add_executable (deltaRad deltaRad.cxx)
add_executable (deltaRad_pi0 deltaRad_pi0.cxx)
#add_executable (deltaRad_template deltaRad_template.cxx)
add_executable (pigLEE pigLEE.cxx)

#target_link_libraries (deltaRad tinyxmllib deltaRadlib ${ROOT_LIBRARIES})
#target_link_libraries (gLEE tinyxmllib deltaRadlib ${ROOT_LIBRARIES})
target_link_libraries (deltaRad_pi0 tinyxmllib deltaRadlib ${ROOT_LIBRARIES})
#target_link_libraries (deltaRad_template tinyxmllib deltaRadlib ${ROOT_LIBRARIES})
target_link_libraries (pigLEE tinyxmllib deltaRadlib ${ROOT_LIBRARIES})



