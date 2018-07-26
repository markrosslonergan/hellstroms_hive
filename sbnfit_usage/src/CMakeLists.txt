# "demo.cxx" and "demo_b.cxx". The extensions are automatically found.

add_executable (delta delta_bounds.cxx)
add_executable (delta_eff delta_bounds_eff.cxx)
add_executable (delta_BF delta_bound_BF.cxx)
add_executable (delta_data delta_bounds_5e19.cxx)
#add_executable (potscale pot_scaling.cxx)

target_link_libraries (delta SBNfitlib tinyxmllib ${ROOT_LIBRARIES})
target_link_libraries (delta_eff SBNfitlib tinyxmllib ${ROOT_LIBRARIES})
target_link_libraries (delta_BF SBNfitlib tinyxmllib ${ROOT_LIBRARIES})
target_link_libraries (delta_data SBNfitlib tinyxmllib ${ROOT_LIBRARIES})
#target_link_libraries (potscale SBNfitlib tinyxmllib ${ROOT_LIBRARIES})



