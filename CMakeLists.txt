cmake_minimum_required(VERSION 2.8.9)

project(MarginCalculator)

#-----------------------------------------------------------------------------
set(EXTENSION_NAME MarginCalculator)

# ------------------------------------------------------------------------
set(MARGINCALCULATOR_VERSION_MAJOR "0")
set(MARGINCALCULATOR_VERSION_MINOR “5”)
set(MARGINCALCULATOR_VERSION_PATCH "0")
set(MARGINCALCULATOR_VERSION ${MARGINCALCULATOR_VERSION_MAJOR}.${MARGINCALCULATOR_VERSION_MINOR}.${MARGINCALCULATOR_VERSION_PATCH})

#-----------------------------------------------------------------------------
set(EXTENSION_HOMEPAGE "http://www.slicer.org/slicerWiki/index.php/Documentation/Nightly/Extensions/MarginCalculator")
set(EXTENSION_CATEGORY “Radiotherapy”)
set(EXTENSION_CONTRIBUTORS "Kevin Wang (Princess Margaret Cancer Centre)")
set(EXTENSION_DESCRIPTION "Extension for Radiotherapy Margin Calculation. Features include synthetic dose volume generation, motion simulation for systematic and random errors, dose population histogram based on motion simulation and complete simulation module for margin calculation. version ${MARGINCALCULATOR_VERSION}")
set(EXTENSION_ICONURL "http://slicer.org/slicerWiki/images/1/11/MarginCalculator_Logo2_128.png")
set(EXTENSION_SCREENSHOTURLS "http://slicer.org/slicerWiki/images/5/53/MarginCalculatorScreenshot1.png")
set(EXTENSION_STATUS "Beta")
set(EXTENSION_DEPENDS "NA") # Specified as a space separated list or 'NA' if any

#-----------------------------------------------------------------------------
find_package(Slicer REQUIRED)
include(${Slicer_USE_FILE})

#-----------------------------------------------------------------------------
#FIND_PACKAGE(SlicerRT REQUIRED)
#INCLUDE(${SlicerRT_USE_FILE})

#-----------------------------------------------------------------------------
OPTION(MARGINCALCULATOR_ENABLE_EXPERIMENTAL_MODULES "Enable the building of work-in-progress, experimental modules." OFF)
mark_as_advanced(MARGINCALCULATOR_ENABLE_EXPERIMENTAL_MODULES)

add_subdirectory(MarginCalculatorCommon)

add_subdirectory(MotionSimulatorDoubleArray)
add_subdirectory(MotionSimulator)
add_subdirectory(DosePopulationHistogram)
add_subdirectory(DoseMorphology)
add_subdirectory(MarginCalculator)
add_subdirectory(SyntheticRTDose)

if (MARGINCALCULATOR_ENABLE_EXPERIMENTAL_MODULES)
  add_subdirectory(DoseVolumeComparison)
  add_subdirectory(DoseVolumeConvolution)
endif()

if(BUILD_TESTING)
  add_subdirectory(Testing)
endif()

#-----------------------------------------------------------------------------
set(CPACK_INSTALL_CMAKE_PROJECTS "${CPACK_INSTALL_CMAKE_PROJECTS};${CMAKE_BINARY_DIR};${EXTENSION_NAME};ALL;/")
#set(CPACK_INSTALL_CMAKE_PROJECTS "${CPACK_INSTALL_CMAKE_PROJECTS};${<EP>_DIR};<EP>;RuntimeLibraries;/")
INCLUDE(${Slicer_EXTENSION_CPACK})
