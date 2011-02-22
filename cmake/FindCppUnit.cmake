# Find the CPPUNIT Library

# CPPUNIT_INCLUDE_DIR - ilm base headers location
# CPPUNIT_LIBRARIES - ilm base libraries
# CPPUNIT_FOUND - true if found

# Mark the key variables as advanced so that they don't convolute the cmake
# gui. These variables are dervied from the CppUnit_Inc_Dir and CppUnit_Lib_Dir
# variables set by the user. 
MARK_AS_ADVANCED (CPPUNIT_INCLUDE_DIR CPPUNIT_LIBRARY)

# Find the headers
find_path (CPPUNIT_INCLUDE_DIR
  cppunit/TestRunner.h
  PATHS ${CppUnit_Inc_Dir}
  )

# Find the libraries
find_library (CPPUNIT_LIBRARY cppunit
  HINTS ${CPPUNIT_INCLUDE_DIR}/../lib
  PATHS ${CppUnit_Lib_Dir}
  )

# handle the QUIETLY and REQUIRED arguments and set CPPUNIT_FOUND to TRUE if 
# all listed variables are TRUE
include (FindPackageHandleStandardArgs) 
find_package_handle_standard_args (
  CppUnit
  "

  CppUnit (http://sourceforge.net/projects/cppunit/) not found.
  Please adjust values for CppUnit_Inc_Dir and CppUnit_Lib_dir.
  CppUnit_Inc_Dir - is the path to the location of the cppunit include
                    directory.
  CppUnit_Lib_Dir - is the path to the location of the cppunit library. If this
                    is located in the path: \"CppUnit_Inc_Dir/../lib\", then 
                    you don't need to set this variable.

  If you have questions concerning building the library, please contact us at:
  code@nektarfx.com

  "
  CPPUNIT_LIBRARY CPPUNIT_INCLUDE_DIR
  )

# Set the CPPUNIT_LIBRARIES variable once we have found the required cppunit
# libraries. 
if (CPPUNIT_FOUND) 
  set (CPPUNIT_LIBRARIES ${CPPUNIT_LIBRARY})
endif (CPPUNIT_FOUND)

