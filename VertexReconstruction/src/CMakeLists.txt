include_directories (${CMAKE_SOURCE_DIR}/inc/)
include_directories (${CMAKE_SOURCE_DIR}/GeoAlgo/)

ROOT_GENERATE_DICTIONARY(StorageDict ${CMAKE_SOURCE_DIR}/inc/Storage.h LINKDEF LinkDef.h)

add_library (VertexReconstructionlib Analyzer.cxx DetectorObjects.cxx FillTreeVariables.cxx GetPermutations.cxx ParticleAssociations.cxx Processor.cxx Storage.cxx StorageDict.cxx VertexBuilder.cxx VertexQuality.cxx VRAnalyzer.cxx)
target_link_libraries (VertexReconstructionlib GeoAlgolib ${ROOT_LIBRARIES})

message(STATUS "ROOT_LIBRARIES @: ${ROOT_LIBRARIES}")

add_executable (VertexReconstruction VertexReconstruction.cxx)
target_link_libraries (VertexReconstruction VertexReconstructionlib ${ROOT_LIBRARIES})

add_executable (Pandora Pandora.cxx)
target_link_libraries (Pandora VertexReconstructionlib ${ROOT_LIBRARIES})

add_executable (RunVertexQuality RunVertexQuality.cxx)
target_link_libraries (RunVertexQuality VertexReconstructionlib ${ROOT_LIBRARIES})

add_executable (GetPermutationFiles GetPermutationFiles.cxx GetPermutations.cxx)
target_link_libraries (GetPermutationFiles ${ROOT_LIBRARIES})

add_executable (RunEvaluateVertexQuality RunEvaluateVertexQuality.cxx EvaluateVertexQuality.cxx)
target_link_libraries (RunEvaluateVertexQuality ${ROOT_LIBRARIES})
