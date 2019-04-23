include_directories (${CMAKE_SOURCE_DIR}/inc/)
include_directories ( ${CMAKE_SOURCE_DIR}/tinyxml/)


add_library (deltaRadlib bdt_eff.cxx variable_list.cxx bdt_datamc.cxx bdt_varplot.cxx bdt_spec.cxx bdt_sig.cxx bdt_recomc.cxx bdt_response.cxx bdt_app.cxx bdt_train.cxx bdt_file.cxx load_mva_param.cxx bdt_precalc.cxx TruncMean.cxx bdt_boxcut.cxx bdt_test.cxx bdt_vertex_eff.cxx)
target_link_libraries (deltaRadlib tinyxmllib ${ROOT_LIBRARIES})

message(STATUS "ROOT_LIBRARIES @: ${ROOT_LIBRARIES}")

#add_executable (gLEE gLEE.cxx)
add_executable (eLEE eLEE.cxx)
#add_executable (deltaRad deltaRad.cxx)
#add_executable (deltaRad_pi0 deltaRad_pi0.cxx)
#add_executable (deltaRad_template deltaRad_template.cxx)
#add_executable (validate validate.cxx)
#add_executable (zvalidate zvalidate.cxx)
#add_executable (invar invarTest.cxx)
add_executable (pigLEE pigLEE.cxx)

#target_link_libraries (deltaRad tinyxmllib deltaRadlib ${ROOT_LIBRARIES})
#target_link_libraries (gLEE tinyxmllib deltaRadlib ${ROOT_LIBRARIES})
target_link_libraries (eLEE tinyxmllib deltaRadlib ${ROOT_LIBRARIES})
#target_link_libraries (validate tinyxmllib deltaRadlib ${ROOT_LIBRARIES})
#target_link_libraries (zvalidate tinyxmllib deltaRadlib ${ROOT_LIBRARIES})
#target_link_libraries (deltaRad_pi0 tinyxmllib deltaRadlib ${ROOT_LIBRARIES})
#target_link_libraries (deltaRad_template tinyxmllib deltaRadlib ${ROOT_LIBRARIES})
#target_link_libraries (invar tinyxmllib deltaRadlib ${ROOT_LIBRARIES})
target_link_libraries (pigLEE tinyxmllib deltaRadlib ${ROOT_LIBRARIES})
