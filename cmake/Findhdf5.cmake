# Find the HDF5 Library

# HDF5_INCLUDE_DIR - hdf5 base headers location
# HDF5_LIBRARIES - hdf5 base libraries
# HDF5_FOUND - true if found

# Mark the key variables as advanced so that they don't convolute the cmake
# gui. These variables are dervied from the hdf5_inc_dir and hdf5_lib_dir
# variables set by the user. 
MARK_AS_ADVANCED (HDF5_INCLUDE_DIR HDF5_LIBRARY)

# Find the headers
find_path (HDF5_INCLUDE_DIR
  hdf5.h
  PATHS ${hdf5_inc_dir}
  )

# Find the libraries
find_library (HDF5_LIBRARY hdf5
  HINTS ${HDF5_INCLUDE_DIR}/../lib
  PATHS ${hdf5_lib_dir}
  )

# handle the QUIETLY and REQUIRED arguments and set HDF5_FOUND to TRUE if 
# all listed variables are TRUE
include (FindPackageHandleStandardArgs) 
find_package_handle_standard_args (
  hdf5
  "

  hdf5 (http://www.hdfgroup.org/HDF5/) not found.
  Please adjust values for hdf5_inc_dir and hdf5_lib_dir.
  hdf5_inc_dir - is the path to the location of the hdf5 include directory.
  hdf5_lib_dir - is the path to the location of the hdf5 library. If this
                 is located in the path: \"hdf5_inc_dir/../lib\", then 
                 you don't need to set this variable.

  If you have questions concerning building the library, please contact us at:
  code@nektarfx.com

  "
  HDF5_LIBRARY HDF5_INCLUDE_DIR
  )

# Set the HDF5_LIBRARIES variable once we have found the required cppunit
# libraries. 
if (HDF5_FOUND) 
  set (HDF5_LIBRARIES ${HDF5_LIBRARY})
endif (HDF5_FOUND)

