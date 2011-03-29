cmake_minimum_required (VERSION 2.8)
project (nkhive)

# version number and definitions 
#-------------------------------------------------------------------------------

set (nkhive_VERSION_MAJOR 0)
set (nkhive_VERSION_MINOR 0)
set (nkhive_VERSION_DEV 1)
set (nkhive_VERSION
  "${nkhive_VERSION_MAJOR}.${nkhive_VERSION_MINOR}.${nkhive_VERSION_DEV}")

# add definitions to use in project.
add_definitions (-DVERSION_MAJOR="${nkhive_VERSION_MAJOR}")
add_definitions (-DVERSION_MINOR="${nkhive_VERSION_MINOR}")
add_definitions (-DVERSION_DEV="${nkhive_VERSION_DEV}")
add_definitions (-DVERSION="${nkhive_VERSION}")

# Default values for CMAKE_BUILD_TYPE
#-------------------------------------------------------------------------------

if (NOT CMAKE_BUILD_TYPE)
  set (CMAKE_BUILD_TYPE Debug CACHE STRING
    "Choose the type of build, options are: Debug, Release" FORCE)
endif (NOT CMAKE_BUILD_TYPE)

if ((NOT CMAKE_BUILD_TYPE MATCHES Debug) AND 
    (NOT CMAKE_BUILD_TYPE MATCHES Release))
  message (FATAL_ERROR "Invalid CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
endif ((NOT CMAKE_BUILD_TYPE MATCHES Debug) AND 
       (NOT CMAKE_BUILD_TYPE MATCHES Release))

# Compiler Flags 
#-------------------------------------------------------------------------------

# Add common flags.
set (CMAKE_CXX_FLAGS
     "-Wall -Wextra -ansi -fno-common -mfix-and-continue" 
     CACHE STRING "" FORCE)

# set debug flags
set (CMAKE_CXX_FLAGS_DEBUG "-O0 -g -DDEBUG" CACHE STRING "" FORCE)

# set release flags
set (CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG" CACHE STRING "" FORCE)

# Additional options
#-------------------------------------------------------------------------------

# Shared or static library? 
option (BUILD_SHARED "Build shared library?" ON)

# Building unittests?
option (BUILD_UNITTESTS "Build unittests?" ON)

# Extra modules for cmake
#-------------------------------------------------------------------------------

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)

# add common cmake file
include (common)

# Options for setting library base paths
#------------------------------------------------------------------------------

# Options for setting Ilm base path
set (IlmBase_Inc_Dir "" CACHE PATH 
     "Location of IlmBase include directory")
set (IlmBase_Lib_Dir "" CACHE PATH 
     "Location of IlmBase libraries")

# Options for setting the nkbase path
set (nkbase_inc_dir "" CACHE PATH
     "Location of nkbase include directory")
set (nkbase_lib_dir "" CACHE PATH
     "Location of nkbase libraries")

# Options for setting the hdf5 path
set (hdf5_inc_dir "" CACHE PATH
     "Location of hdf5 include directory")
set (hdf5_lib_dir "" CACHE PATH
     "Location of hdf5 libraries")

# Options for setting the cppunit path
set (CppUnit_Inc_Dir "" CACHE PATH 
      "Location of CppUnit include directory, if building unittests")
set (CppUnit_Lib_Dir "" CACHE PATH 
      "Location of CppUnit library, if building unittests")

# Find Necessary Packages
#-------------------------------------------------------------------------------

# Find boost headers
set (Boost_USE_MULTITHREADED ON)
find_package (Boost REQUIRED COMPONENTS thread)
include_directories (${Boost_INCLUDE_DIRS})

# Make sure we have ilm base
find_package (IlmBase REQUIRED)
include_directories (${ILMBASE_INCLUDE_DIR})

# Make sure we have nkbase
find_package (nkbase REQUIRED)
include_directories (${NKBASE_INCLUDE_DIR})

# Make sure we have hdf5
find_package (hdf5 REQUIRED)
include_directories (${HDF5_INCLUDE_DIR})

# Subdirectories
#-------------------------------------------------------------------------------

add_subdirectory (src)

# Stuff needed for Unit tests
#-------------------------------------------------------------------------------

if (${BUILD_UNITTESTS})

  # Add definitions needed for unittets.
  add_definitions (-DUNITTEST)

  # Find the cppunit package
  find_package (CppUnit REQUIRED)
  include_directories (${CPPUNIT_INCLUDE_DIR})

  # add the unittest directory to build.
  add_subdirectory (unittest)

endif (${BUILD_UNITTESTS})

#-------------------------------------------------------------------------------
