cmake_minimum_required(VERSION 2.8.9)

project(MarginCalculator)

#-----------------------------------------------------------------------------
set(EXTENSION_NAME MarginCalculator)

# ------------------------------------------------------------------------
set(MARGINCALCULATOR_VERSION_MAJOR "0")
set(MARGINCALCULATOR_VERSION_MINOR "1")
set(MARGINCALCULATOR_VERSION_PATCH "0")
set(MARGINCALCULATOR_VERSION ${MARGINCALCULATOR_VERSION_MAJOR}.${MARGINCALCULATOR_VERSION_MINOR}.${MARGINCALCULATOR_VERSION_PATCH})

#-----------------------------------------------------------------------------
set(EXTENSION_HOMEPAGE "http://www.slicer.org/slicerWiki/index.php/Documentation/Nightly/Extensions/MarginCalculator")
set(EXTENSION_CATEGORY "MARGIN CALCULATOR")
set(EXTENSION_CONTRIBUTORS "Kevin Wang (Radiation Medicine Program, Princess Margaret Cancer Centre, University Health Network, Toronto, Canada)")
set(EXTENSION_DESCRIPTION "Extensions for RT Margin Calculator version ${MARGINCALCULATOR_VERSION}")
set(EXTENSION_ICONURL "http://wiki.slicer.org/slicerWiki/images/f/f2/MARGINCALCULATORExtensionLogo.png")
set(EXTENSION_SCREENSHOTURLS "http://www.slicer.org/slicerWiki/images/0/04/MARGINCALCULATOR0.3_Screenshot_DVH_HeadNeck_3.png http://www.slicer.org/slicerWiki/images/c/c0/MARGINCALCULATOR0.4_Screenshot_Isodose.png http://www.slicer.org/slicerWiki/images/2/20/MARGINCALCULATOR0.4_Screenshot_Gamma.png")
set(EXTENSION_STATUS "Beta")
set(EXTENSION_DEPENDS "NA") # Specified as a space separated list or 'NA' if any

#-----------------------------------------------------------------------------
find_package(Slicer REQUIRED)
include(${Slicer_USE_FILE})

#-----------------------------------------------------------------------------
#FIND_PACKAGE(SlicerRT REQUIRED)
#INCLUDE(${SlicerRT_USE_FILE})

#-----------------------------------------------------------------------------
OPTION(MARGINCALCULATOR_ENABLE_EXPERIMENTAL_MODULES "Enable the building of work-in-progress, experimental modules." ON)
mark_as_advanced(MARGINCALCULATOR_ENABLE_EXPERIMENTAL_MODULES)

add_subdirectory(SlicerRtCommon)

add_subdirectory(MotionSimulatorDoubleArray)
add_subdirectory(MotionSimulator)
add_subdirectory(DosePopulationHistogram)
add_subdirectory(DoseMorphology)
add_subdirectory(MarginCalculator)

if (MARGINCALCULATOR_ENABLE_EXPERIMENTAL_MODULES)
  add_subdirectory(DoseVolumeConvolution)
  add_subdirectory(DoseVolumeComparison)
endif()

if(BUILD_TESTING)
  add_subdirectory(Testing)
endif()

#-----------------------------------------------------------------------------
set(CPACK_INSTALL_CMAKE_PROJECTS "${CPACK_INSTALL_CMAKE_PROJECTS};${CMAKE_BINARY_DIR};${EXTENSION_NAME};ALL;/")
#set(CPACK_INSTALL_CMAKE_PROJECTS "${CPACK_INSTALL_CMAKE_PROJECTS};${<EP>_DIR};<EP>;RuntimeLibraries;/")
INCLUDE(${Slicer_EXTENSION_CPACK})
