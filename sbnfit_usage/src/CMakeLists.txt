# "demo.cxx" and "demo_b.cxx". The extensions are automatically found.

add_executable (delta delta_bounds.cxx)

target_link_libraries (delta SBNfitlib tinyxmllib ${ROOT_LIBRARIES})


