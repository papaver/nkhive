# Find the ILM Base library. 

# ILMBASE_INCLUDE_DIR - ilm base headers location
# ILMBASE_LIBRARIES - ilm base libraries
# ILMBASE_FOUND - true if found

# Mark the key variables as advanced so that they don't convolute the cmake
# gui. These variables are dervied from the IlmBase_Inc_Dir and IlmBase_Lib_Dir
# variables set by the user. 
MARK_AS_ADVANCED (ILMBASE_INCLUDE_DIR 
                  ILMBASE_ex_LIBRARY
                  ILMBASE_math_LIBRARY
                  ILMBASE_half_LIBRARY)

# Find the headers
find_path (ILMBASE_INCLUDE_DIR
  OpenEXR/ImathMath.h
  PATHS ${IlmBase_Inc_Dir}
  )

# Find the libraries
find_library (ILMBASE_ex_LIBRARY Iex
  HINTS ${ILMBASE_INCLUDE_DIR}/../lib
  PATHS ${IlmBase_Lib_Dir}
  )

find_library (ILMBASE_math_LIBRARY Imath
  HINTS ${ILMBASE_INCLUDE_DIR}/../lib
  PATHS ${IlmBase_Lib_Dir}
  )

find_library (ILMBASE_half_LIBRARY Half
  HINTS ${ILMBASE_INCLUDE_DIR}/../lib
  PATHS ${IlmBase_Lib_Dir}
  )

# handle the QUIETLY and REQUIRED arguments and set ILMBASE_FOUND to
# TRUE if all listed variables are TRUE
include (FindPackageHandleStandardArgs)
find_package_handle_standard_args (
  IlmBase
  "

  IlmBase (http://www.openexr.com/) not found. Use ccmake to configure paths.
  Adjust values for IlmBase_Inc_Dir and IlmBase_Lib_Dir.
  IlmBase_Inc_Dir - is the path to the location of the ilmbase include
                    directory.
  IlmBase_Lib_Dir - is the path to the location of the Iex, Imath, and
                    Half libraries. If this is located in the path:
                    \"IlmBase_Inc_Dir/../lib\", then you don't need to set this
                    variable.

  If you have questions concerning building the library, please contact us at:
  code@nektarfx.com

  "
  ILMBASE_INCLUDE_DIR 
  ILMBASE_math_LIBRARY ILMBASE_ex_LIBRARY ILMBASE_half_LIBRARY)

# Set the ILMBASE_LIBRARIES variable once we have found the required ilmbase
# libraries. 
if (ILMBASE_FOUND)
  set (ILMBASE_LIBRARIES ${ILMBASE_ex_LIBRARY} ${ILMBASE_half_LIBRARY})
  set (ILMBASE_LIBRARIES ${ILMBASE_LIBRARIES} ${ILMBASE_math_LIBRARY})
endif (ILMBASE_FOUND)
