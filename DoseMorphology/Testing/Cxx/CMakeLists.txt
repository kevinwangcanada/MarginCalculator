set(KIT qSlicer${MODULE_NAME}Module)

#-----------------------------------------------------------------------------
set(KIT_TEST_SRCS)
set(KIT_TEST_NAMES)
set(KIT_TEST_NAMES_CXX)
SlicerMacroConfigureGenericCxxModuleTests(${MODULE_NAME} KIT_TEST_SRCS KIT_TEST_NAMES KIT_TEST_NAMES_CXX)

#-----------------------------------------------------------------------------
#set(CMAKE_TESTDRIVER_BEFORE_TESTMAIN "DEBUG_LEAKS_ENABLE_EXIT_ERROR();" )
create_test_sourcelist(Tests ${KIT}CxxTests.cxx
  ${KIT_TEST_NAMES_CXX}
  vtkSlicerDoseMorphologyModuleLogicTest1.cxx
  # EXTRA_INCLUDE vtkMRMLcontourNode.h
  )
list(REMOVE_ITEM Tests ${KIT_TEST_NAMES_CXX})
list(APPEND Tests ${KIT_TEST_SRCS})

#-----------------------------------------------------------------------------
add_executable(${KIT}CxxTests ${Tests})
set_target_properties(${KIT}CxxTests PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${Slicer_BIN_DIR})
target_link_libraries(${KIT}CxxTests ${KIT})

#-----------------------------------------------------------------------------
foreach(testname ${KIT_TEST_NAMES})
  SIMPLE_TEST( ${testname} )
endforeach()

# Add your test after this line, using SIMPLE_TEST( <testname> )
#-----------------------------------------------------------------------------
set(TEMP "${CMAKE_BINARY_DIR}/Testing/Temporary")

macro(TEST_WITH_DATA TestName TestExecutableName
      DataDirectoryPath ReferenceDoseVolumeFile
      InputDoseVolumeFile BaselineDoseVolumeFile 
      TemporarySceneFile MorphologicalOperation
      MorphologicalParameter VolumeDifferenceToleranceVoxel)
  add_test(
    NAME ${TestName}
    COMMAND ${Slicer_LAUNCH_COMMAND} $<TARGET_FILE:${KIT}CxxTests> ${TestExecutableName} ${ARGN}
    -DataDirectoryPath ${DataDirectoryPath}
    -ReferenceDoseVolumeFile  ${ReferenceDoseVolumeFile}
    -InputDoseVolumeFile  ${InputDoseVolumeFile}
    -BaselineDoseVolumeFile  ${BaselineDoseVolumeFile}
    -TemporarySceneFile ${TemporarySceneFile}
    -MorphologicalOperation ${MorphologicalOperation}
    -MorphologicalParameter ${MorphologicalParameter}
    -VolumeDifferenceToleranceVoxel ${VolumeDifferenceToleranceVoxel}
  )
endmacro()

#-----------------------------------------------------------------------------
TEST_WITH_DATA(
  vtkSlicerDoseMorphologyModuleLogicTest_EclipseProstate_ExpandByScaling
  vtkSlicerDoseMorphologyModuleLogicTest1
  ${CMAKE_CURRENT_SOURCE_DIR}/../../Data/EclipseProstate/
  RTDose.nrrd
  RTDose.nrrd
  RTDose_ExpandByScaling_Baseline.nrrd
  ${TEMP}/TestScene_ContourMorphology_EclipseProstate.mrml
  ExpandByScaling
  1.1
  100.0
)
set_tests_properties(vtkSlicerDoseMorphologyModuleLogicTest_EclipseProstate_ExpandByScaling 
  PROPERTIES FAIL_REGULAR_EXPRESSION "Error;Warning" )

#-----------------------------------------------------------------------------
TEST_WITH_DATA(
  vtkSlicerDoseMorphologyModuleLogicTest_EclipseProstate_ExpandByDilation
  vtkSlicerDoseMorphologyModuleLogicTest1
  ${CMAKE_CURRENT_SOURCE_DIR}/../../Data/EclipseProstate/
  RTDose.nrrd
  RTDose.nrrd
  RTDose_ExpandByDilation_Baseline.nrrd
  ${TEMP}/TestScene_ContourMorphology_EclipseProstate.mrml
  ExpandByDilation
  1.0
  100.0
)
set_tests_properties(vtkSlicerDoseMorphologyModuleLogicTest_EclipseProstate_ExpandByDilation 
  PROPERTIES FAIL_REGULAR_EXPRESSION "Error;Warning" )