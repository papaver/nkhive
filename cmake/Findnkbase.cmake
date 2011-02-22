# Find the NK Base library. 

# NKBASE_INCLUDE_DIR - ilm base headers location
# NKBASE_LIBRARIES - ilm base libraries
# NKBASE_FOUND - true if found

# Mark the key variables as advanced so that they don't convolute the cmake
# gui. These variables are dervied from the nkbase_inc_dir and nkbase_lib_dir
# variables set by the user. 
MARK_AS_ADVANCED (NKBASE_INCLUDE_DIR 
                  NKBASE_LIBRARY)

# Find the headers
find_path (NKBASE_INCLUDE_DIR
  nkbase/Types.h
  PATHS ${nkbase_inc_dir}
  )

# Find the libraries
find_library (NKBASE_LIBRARY nkbase
  HINTS ${NKBASE_INCLUDE_DIR}/../lib
  PATHS ${nkbase_lib_dir}
  )

# handle the QUIETLY and REQUIRED arguments and set NKBASE_FOUND to
# TRUE if all listed variables are TRUE
include (FindPackageHandleStandardArgs)
find_package_handle_standard_args (
  nkbase
  "

  nkbase (http://www.nektarfx.com/nkbase) not found. Use ccmake to configure 
  paths.  Adjust values for nkbase_inc_dir and nkbase_lib_dir.
  nkbase_inc_dir - is the path to the location of the nkbase include
                   directory.
  nkbase_lib_dir - is the path to the location of the nkbase library. 
                   If this is located in the path: \"nkbase_inc_dir/../lib\", 
                   then you don't need to set this variable.

  If you have questions concerning building the library, please contact us at:
  code@nektarfx.com

  "
  NKBASE_INCLUDE_DIR NKBASE_LIBRARY
  )

# Set the NKBASE_LIBRARIES variable once we have found the required ilmbase
# libraries. 
if (NKBASE_FOUND)
  set (NKBASE_LIBRARIES ${NKBASE_LIBRARY})
endif (NKBASE_FOUND)
