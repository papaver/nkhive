
set (Directories 
  .
  )

append_files (Sources "cpp" ${Directories})

include_directories (BEFORE ${PROJECT_SOURCE_DIR}/src)

add_executable (unittest ${Sources})

target_link_libraries (unittest nkhive ${ILMBASE_LIBRARIES} 
                                       ${CPPUNIT_LIBRARIES}
                                       ${NKBASE_LIBRARIES}
                                       ${HDF5_LIBRARIES}
                                       ${Boost_THREAD_LIBRARY})
