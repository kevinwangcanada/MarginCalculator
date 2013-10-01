set(KIT qSlicer${MODULE_NAME}Module)

set(KIT_TEST_SRCS)
set(KIT_TEST_NAMES)
set(KIT_TEST_NAMES_CXX)
SlicerMacroConfigureGenericCxxModuleTests(${MODULE_NAME} KIT_TEST_SRCS KIT_TEST_NAMES KIT_TEST_NAMES_CXX)

set(CMAKE_TESTDRIVER_BEFORE_TESTMAIN "DEBUG_LEAKS_ENABLE_EXIT_ERROR();" )
create_test_sourcelist(Tests ${KIT}CxxTests.cxx
  ${KIT_TEST_NAMES_CXX}
  # vtkSlicerDosePopulationHistogramModuleLogicTest1.cxx
  EXTRA_INCLUDE vtkMRMLDebugLeaksMacro.h
  )

list(REMOVE_ITEM Tests ${KIT_TEST_NAMES_CXX})
list(APPEND Tests ${KIT_TEST_SRCS})

add_executable(${KIT}CxxTests ${Tests})
target_link_libraries(${KIT}CxxTests ${KIT})

foreach(testname ${KIT_TEST_NAMES})
  SIMPLE_TEST( ${testname} )
endforeach()

#-----------------------------------------------------------------------------
set(TEMP "${CMAKE_BINARY_DIR}/Testing/Temporary")

macro(TEST_WITH_DATA TestName TestExecutableName
      DataDirectoryPath BaselineDvhTableCsvFile BaselineDvhMetricCsvFile
      TemporarySceneFile TemporaryDvhTableCsvFile TemporaryDvhMetricCsvFile
      VolumeDifferenceCriterion DoseToAgreementCriterion AgreementAcceptancePercentageThreshold MetricDifferenceThreshold
      DvhStartValue DvhStepSize RasterizationDownsamplingFactor)
  add_test(
    NAME ${TestName}
    COMMAND ${Slicer_LAUNCH_COMMAND} $<TARGET_FILE:${KIT}CxxTests> ${TestExecutableName} ${ARGN}
    -DataDirectoryPath ${DataDirectoryPath}
    -BaselineDvhTableCsvFile ${BaselineDvhTableCsvFile}
    -BaselineDvhMetricCsvFile ${BaselineDvhMetricCsvFile}
    -TemporarySceneFile ${TemporarySceneFile}
    -TemporaryDvhTableCsvFile ${TemporaryDvhTableCsvFile}
    -TemporaryDvhMetricCsvFile ${TemporaryDvhMetricCsvFile}
    -VolumeDifferenceCriterion ${VolumeDifferenceCriterion}
    -DoseToAgreementCriterion ${DoseToAgreementCriterion}
    -AgreementAcceptancePercentageThreshold ${AgreementAcceptancePercentageThreshold}
    -MetricDifferenceThreshold ${MetricDifferenceThreshold}
    -DvhStartValue ${DvhStartValue}
    -DvhStepSize ${DvhStepSize}
    -RasterizationDownsamplingFactor ${RasterizationDownsamplingFactor}
  )
endmacro()

#-----------------------------------------------------------------------------
TEST_WITH_DATA(
  vtkSlicerDosePopulationHistogramModuleLogicTest_EclipseProstate
  vtkSlicerDosePopulationHistogramModuleLogicTest1
  ${CMAKE_CURRENT_SOURCE_DIR}/../../Data/EclipseProstate
  ${CMAKE_CURRENT_SOURCE_DIR}/../../Data/EclipseProstate/DvhTable_SlicerRT.csv
  ${CMAKE_CURRENT_SOURCE_DIR}/../../Data/EclipseProstate/DvhMetrics_SlicerRT.csv
  ${TEMP}/TestScene_EclipseProstate.mrml
  ${TEMP}/TestDvhTable_EclipseProstate_SlicerRT.csv
  ${TEMP}/TestDvhMetrics_EclipseProstate_SlicerRT.csv
  0.0
  0.0
  100.0
  0.0
  0.0
  0.0
  2.0
)
set_tests_properties(vtkSlicerDosePopulationHistogramModuleLogicTest_EclipseProstate PROPERTIES FAIL_REGULAR_EXPRESSION "Error;Warning" )

#-----------------------------------------------------------------------------
TEST_WITH_DATA(
  vtkSlicerDosePopulationHistogramModuleLogicTest_EclipseProstate_CERR
  vtkSlicerDosePopulationHistogramModuleLogicTest1
  ${CMAKE_CURRENT_SOURCE_DIR}/../../Data/EclipseProstate
  ${CMAKE_CURRENT_SOURCE_DIR}/../../Data/EclipseProstate/DvhTable_CERR.csv
  ${CMAKE_CURRENT_SOURCE_DIR}/../../Data/EclipseProstate/NoMetricComparison
  ${TEMP}/TestScene_EclipseProstate_CERR.mrml
  ${TEMP}/TestDvhTable_EclipseProstate_CERR_SlicerRT.csv
  ${TEMP}/TestDvhMetrics_EclipseProstate_CERR_SlicerRT.csv
  1.0
  1.0
  95.0
  3.0
  0.01
  0.01
  2.0
)
set_tests_properties(vtkSlicerDosePopulationHistogramModuleLogicTest_EclipseProstate_CERR PROPERTIES FAIL_REGULAR_EXPRESSION "Error;Warning" )

