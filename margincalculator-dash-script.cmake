SET (CTEST_SOURCE_DIRECTORY "C:/S/Dashboard/MarginCalculator")
SET (CTEST_BINARY_DIRECTORY "C:/S/Dashboard/MarginCalculatorbin")

SET (CTEST_SVN_COMMAND "C:/Program Files/TortoiseSVN/bin/svn.exe")
SET (CTEST_SVN_CHECKOUT  "${CTEST_SVN_COMMAND} https://subversion.assembla.com/svn/margincalculator/trunk/MarginCalculator co -d\"${CTEST_SOURCE_DIRECTORY}\" CMake")

# which ctest command to use for running the dashboard
SET (CTEST_COMMAND 
  "C:/wangk/bin/cmake-2.8.12.2-win32-x86/bin/ctest.exe -D Nightly -C RelWithDebInfo"
  )

# what cmake command to use for configuring this dashboard
SET (CTEST_CMAKE_COMMAND 
  "C:/wangk/bin/cmake-2.8.12.2-win32-x86/bin/cmake.exe"
  )
  
SET (CTEST_CONFIGURATION_TYPE "RelWithDebInfo")
SET (CTEST_BUILD_CONFIGURATION "RelWithDebInfo")


####################################################################
# The values in this section are optional you can either
# have them or leave them commented out
####################################################################

# should ctest wipe the binary tree before running
SET (CTEST_START_WITH_EMPTY_BINARY_DIRECTORY TRUE)

# this is the initial cache to use for the binary tree, be careful to escape
# any quotes inside of this string if you use it
SET (CTEST_INITIAL_CACHE "
MAKECOMMAND:STRING=C:/Program Files (x86)/Microsoft Visual Studio 9.0/Common7/IDE/devenv.com MarginCalculator.sln /build RelWithDebInfo /project ALL_BUILD
CMAKE_MAKE_PROGRAM:FILEPATH=C:/Program Files (x86)/Microsoft Visual Studio 9.0/Common7/IDE/devenv.com
CMAKE_GENERATOR:INTERNAL=Visual Studio 9 2008 Win64
BUILDNAME:STRING=Win32-VS2008
SITE:STRING=wangk.rmp.uhn.ca
SVNCOMMAND:FILEPATH=C:/Program Files/TortoiseSVN/bin/svn.exe
Slicer_DIR=C:/S/Vb/Slicer-build
CMAKE_BUILD_TYPE:STRING=RelWithDebInfo
CTEST_CONFIGURATION_TYPE:STRING=RelWithDebInfo
")

# set any extra environment variables to use during the execution of the script here:
SET (CTEST_ENVIRONMENT
)

