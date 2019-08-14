include_directories (${CMAKE_SOURCE_DIR}/inc/)
include_directories ( ${CMAKE_SOURCE_DIR}/tinyxml/)



#add_executable (validate validate.cxx)
add_executable (det_validate det_validate.cxx)
add_executable (set_validate set_validate.cxx)
#add_executable (invar invarTest.cxx)

#target_link_libraries (validate tinyxmllib hiveLib ${ROOT_LIBRARIES})
target_link_libraries (det_validate tinyxmllib hiveLib ${ROOT_LIBRARIES})
target_link_libraries (set_validate tinyxmllib hiveLib ${ROOT_LIBRARIES})
#target_link_libraries (invar tinyxmllib hiveLib ${ROOT_LIBRARIES})
target_link_libraries (pigLEE tinyxmllib hiveLib ${ROOT_LIBRARIES})



