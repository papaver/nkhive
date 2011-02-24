
# Find files and directories to compile and install
#-------------------------------------------------------------------------------

set (Directories 
  .
  attributes
  bitfields
  tiling
  interpolation
  io
  io/hdf5
  util
  volume
  xforms
  )

append_files (Headers "h" ${Directories})
append_files (Inlines "hpp" ${Directories})
append_files (Sources "cpp" ${Directories})

set (LIB_INSTALL_PATH "${CMAKE_INSTALL_PREFIX}/lib")

# Define library to build and it's dependencies
#-------------------------------------------------------------------------------

if (BUILD_SHARED)
  add_library (${PROJECT_NAME} SHARED ${Headers} ${Inlines} ${Sources})
else (BUILD_SHARED)
  add_library (${PROJECT_NAME} ${Headers} ${Inlines} ${Sources})
endif (BUILD_SHARED)

# Set the installation prefix path so that the path is encoded in the library. 
set_target_properties (${PROJECT_NAME} PROPERTIES
                       INSTALL_NAME_DIR ${LIB_INSTALL_PATH}
                       INSTALL_RPATH ${LIB_INSTALL_PATH}
                       VERSION ${nkhive_VERSION})

target_link_libraries (${PROJECT_NAME} ${ILMBASE_LIBRARIES}
                                       ${HDF5_LIBRARIES}
                                       ${Boost_THREAD_LIBRARY})

# What do we install when running 'make install'
#-------------------------------------------------------------------------------

# Copy over the library into the lib directory
install (
  TARGETS ${PROJECT_NAME}
  DESTINATION ${LIB_INSTALL_PATH}
  )

# Copy over all headers into an include directory
foreach (dir ${Directories})
  install (
    DIRECTORY ${dir} 
    DESTINATION ${CMAKE_INSTALL_PREFIX}/include/${PROJECT_NAME}
    PATTERN ${dir}/*.h
    PATTERN ${dir}/*.hpp
  )
endforeach (dir)

#-------------------------------------------------------------------------------
