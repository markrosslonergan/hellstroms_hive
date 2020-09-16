include_directories (${CMAKE_SOURCE_DIR}/inc/)
include_directories ( ${CMAKE_SOURCE_DIR}/tinyxml/)



#add_executable (validate validate.cxx)
add_executable (xgboost_validate xgboost_validate.cxx)
add_executable (optical_filter_match optical_filter_match.cxx)


#target_link_libraries (validate tinyxmllib hiveLib ${ROOT_LIBRARIES})
target_link_libraries (xgboost_validate tinyxmllib hiveLib ${ROOT_LIBRARIES})
target_link_libraries (optical_filter_match tinyxmllib hiveLib ${ROOT_LIBRARIES})


set_output_directory(xgboost_validate ${CMAKE_SOURCE_DIR}/build/bin)
set_output_directory(optical_filter_match ${CMAKE_SOURCE_DIR}/build/bin)

