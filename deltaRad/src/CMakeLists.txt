
include_directories (${CMAKE_SOURCE_DIR}/inc/)

add_library (deltaRadlib train.cxx app.cxx merge.cxx significance.cxx gen_tlimits.cxx load_trees.cxx get_pot_info.cxx get_mva_response_hists.cxx get_vertices_per_event.cxx plot_mva_response_hists.cxx plotstack.cxx)
target_link_libraries (deltaRadlib ${ROOT_LIBRARIES})


message(STATUS "ROOT_LIBRARIES @: ${ROOT_LIBRARIES}")


add_executable (deltaRad deltaRad.cxx)
target_link_libraries (deltaRad deltaRadlib ${ROOT_LIBRARIES})


