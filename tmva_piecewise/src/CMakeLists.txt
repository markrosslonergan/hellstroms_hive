
include_directories (${CMAKE_SOURCE_DIR}/inc/)

add_library (DeltaRadlib train.cxx app.cxx merge.cxx significance.cxx gen_tlimits.cxx load_trees.cxx get_mva_response_hists.cxx get_vertices_per_event.cxx plot_mva_response_hists.cxx)
target_link_libraries (DeltaRadlib ${ROOT_LIBRARIES})


message(STATUS "ROOT_LIBRARIES @: ${ROOT_LIBRARIES}")


add_executable (DeltaRad DeltaRad.cxx)
target_link_libraries (DeltaRad DeltaRadlib ${ROOT_LIBRARIES})


