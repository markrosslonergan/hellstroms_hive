include_directories (${CMAKE_SOURCE_DIR}/inc/)
include_directories ( ${CMAKE_SOURCE_DIR}/tinyxml/)

add_library (deltaRadlib bdt_datamc.cxx bdt_spec.cxx bdt_sig.cxx bdt_recomc.cxx bdt_response.cxx bdt_app.cxx bdt_train.cxx bdt_file.cxx load_mva_param.cxx bdt_precalc.cxx)
target_link_libraries (deltaRadlib tinyxmllib ${ROOT_LIBRARIES})

message(STATUS "ROOT_LIBRARIES @: ${ROOT_LIBRARIES}")


add_executable (deltaRad deltaRad.cxx)
add_executable (deltaRad_pi0 deltaRad_pi0.cxx)
target_link_libraries (deltaRad tinyxmllib deltaRadlib ${ROOT_LIBRARIES})
target_link_libraries (deltaRad_pi0 tinyxmllib deltaRadlib ${ROOT_LIBRARIES})
