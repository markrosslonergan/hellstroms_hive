include_directories (${CMAKE_SOURCE_DIR}/inc/)
include_directories ( ${CMAKE_SOURCE_DIR}/tinyxml/)

add_library (deltaRadlib bdt_eff.cxx variable_list.cxx bdt_datamc.cxx bdt_varplot.cxx bdt_spec.cxx bdt_sig.cxx bdt_recomc.cxx bdt_response.cxx bdt_app.cxx bdt_train.cxx bdt_file.cxx load_mva_param.cxx bdt_precalc.cxx TruncMean.cxx bdt_boxcut.cxx)
target_link_libraries (deltaRadlib tinyxmllib ${ROOT_LIBRARIES})

message(STATUS "ROOT_LIBRARIES @: ${ROOT_LIBRARIES}")

#add_executable (deltaRad3 deltaRad.cxx)
#add_executable (deltaRad_pi0 deltaRad_pi0.cxx)
#add_executable (deltaRad_template deltaRad_template.cxx)
add_executable (deltaRad_electron deltaRad_electron.cxx)
#add_executable (invar invarTest.cxx)

#target_link_libraries (deltaRad3 tinyxmllib deltaRadlib ${ROOT_LIBRARIES})
#target_link_libraries (deltaRad_pi0 tinyxmllib deltaRadlib ${ROOT_LIBRARIES})
#target_link_libraries (deltaRad_template tinyxmllib deltaRadlib ${ROOT_LIBRARIES})
target_link_libraries (deltaRad_electron tinyxmllib deltaRadlib ${ROOT_LIBRARIES})
#target_link_libraries (invar tinyxmllib deltaRadlib ${ROOT_LIBRARIES})

add_library (deltaRadlib_toying bdt_eff.cxx variable_list_toying.cxx bdt_datamc.cxx bdt_varplot.cxx bdt_spec.cxx bdt_sig.cxx bdt_recomc.cxx bdt_response.cxx bdt_app.cxx bdt_train.cxx bdt_file.cxx load_mva_param.cxx bdt_precalc.cxx TruncMean.cxx bdt_boxcut.cxx)
target_link_libraries (deltaRadlib_toying tinyxmllib ${ROOT_LIBRARIES})
add_executable (deltaRad_toying deltaRad_electron.cxx)
target_link_libraries (deltaRad_toying tinyxmllib deltaRadlib_toying ${ROOT_LIBRARIES})
