        NektarFX HiVE (nkhive) release 0.0.1

These are release notes for nkhive version 0.0.1. Read them carefully as they
tell you what this is all about, explaining how to install the library and what
to do if something goes wrong. 

--------------------------------------------------------------------------------

What is nkhive?

    nkhive is an implementation of HiVE (Hierarchical Volume Encapsulation), a
    library developed at NektarFX for storage and processing of large,
    boundless, sparse volumes. 

    It is distributed under the BSD License -- see the accompanying LICENSE.txt
    file for more details.

    To keep up with news and the latest developments on nkhive, you may want to
    subscribe to our rss feed at : http://www.nektarfx.com/nkhive

    If you have questions about using and contributing to NektarFX libraries,
    you can email us at: code@nektarfx.com. 

--------------------------------------------------------------------------------

Contributing 

    The success of the library depends on contributors like yourself. There are
    several ways to contribute to the project, from writing documentation to
    writing language bindings to adding core features. You don't necessarily
    have to be a developer to contribute. 

    For more details on how to contribute to our open source projects, please
    refer to the contribution guide at : http://www.nektarfx.com/contributing

    If you have feature/support requests or general comments, questions, and
    concerns, please contact us at code@nektarfx.com. We are always eager to
    hear from you with feedback that can help improve the project.

--------------------------------------------------------------------------------

Supported Platforms
    
    All flavors of Unix, Linux, and Mac OS X.

    Windows is not currently supported at this time. Although, if you are
    proficient with cmake, minor tweaks can be made to make it ready for a
    window distribution as well.

--------------------------------------------------------------------------------

Documentation

    There is ample documentation available for nkhive on the project website
    and blog at : http://www.nektarfx.com/nkhive. This README is not meant to
    serve as a complete documentation to the system. 

--------------------------------------------------------------------------------

Dependencies

    cmake (version 2.8 or later) - http://www.cmake.org 
    Required for building and installing the library. 

    IlmBase Libraries (required) - http://www.openexr.com
    Required for basic math operations. 
    This version of nkhive was tested against ilmbase version 1.0.2.

    nkbase library (required) - http://www.nektarfx.com/nkbase
    Required for basic NektarFX utilities and tools.
    This version of nkhive was tested against nkbase version 0.0.1

    Boost libraries (required) - http://www.boost.org
    Required for threading support, shared pointers, utilities and algorithms.
    This version of nkhive was tested against boost version 1.40.0

    HDF5 library (required) - http://www.hdfgroup.org/HDF5/
    Required for I/O of hive files. 
    This version of nkhive was tested against hdf5 version 1.8.5

    cppunit library (if building local unittests) -
    http://sourceforge.net/projects/cppunit/
    Required for writing and running unittests. 
    This version of nkhive was tested against cppunit version 1.12.1

--------------------------------------------------------------------------------

Building & Installation

    0. Make sure that you have cmake installed and the cmake binaries are in
       your PATH environment. You will need to run the commands "cmake" and
       "ccmake" to build this library properly.

    1. mkdir build; cd build;
       Make a build directory inside the root of the source package you
       downloaded/cloned for nkhive. All builds should happen here. 

    2. ccmake ..; 
       This will run the cmake gui that can be used to generate make files for
       the project. 

    3. Within the cmake gui, press 'c' to configure the project. If you do not
       get any errors, then make sure that all settings shown are correct.

       By default, unittests are configured to build. However, if you trust the
       developers of the library, don't wish to run/build unittests of your
       own, or don't want to install cppunit, then set the BUILD_UNITTESTS
       setting to OFF.

       To compile with optimizations, you should change the CMAKE_BUILD_TYPE
       setting from "Debug" to "Release".

       You can also change the default install path by changing the
       CMAKE_INSTALL_PREFIX setting to a path you desire. This will be the base
       path. The installation procedure will create an "include" directory and
       a "lib" directory in the path defined here. The "include" directory will
       contain all library headers and the "lib" directory will contain the
       library binaries.

       If you have the ILM base libraries installed in a non-standard location
       and cmake is unable to find them, you may need to tell cmake where it
       can find the ILM base libraries and headers. Change the path for setting
       IlmBase_Inc_Dir to the include directory that contains the library's
       headers. This should be the directory containing the folder 'OpenEXR'.
       Next, change the setting for IlmBase_Lib_Dir to the path where the binary
       shared libraries are located.

       If you have the nkbase library installed in a non-standard location and
       cmake is unable to find it, you will need to tell cmake where it can
       find the nkbase headers and the nkbase library. Change the path for
       setting 'nkbase_inc_dir' to the include directory that contains the
       library's headers. This should be the directory containing the folder
       'nkbase'. Next, change the setting for 'nkbase_lib_dir' to the path
       where the binary shared library is located.

       If you have the hdf5 library installed in a non-standard location and
       cmake is unable to find it, you will need to tell cmake where it can
       find the hdf5 headers and the hdf5 library. Change the path for
       setting 'hdf5_inc_dir' to the include directory that contains the
       library's headers. This should be the directory containing the file
       'hdf5.h'. Next, change the setting for 'hdf5_lib_dir' to the path
       where the binary shared library is located.

       If you have the boost c++ libraries installed in a non-standard location
       you will need to tell cmake where the boost include directory is
       located. To do this, you will have to enter cmake's advanced options.
       You can do this by pressing 't'. This will bring up a list of advanced
       options. Set the path for setting 'Boost_INCLUDE_DIR' to the 'boost'
       directory, which contains boost's header files.

       NOTE: nkhive requires the boost thread library. This can be compiled as
       part of boost using boost's build system. Please refer to
       http://www.boost.org for instructions on compiling boost libraries. If
       this library is not found by cmake, you will have to tell cmake where to
       find it. To do this, enter advanced mode by pressing 't' and then change
       the path for setting 'Boost_THREAD_LIBRARY' to the path where the
       library 'libboost_thread.*' or 'libboost_thread-mt.*' is located.

       If you have BUILD_UNITTESTS set to ON, cmake will check for cppunit. If
       you have cppunit installed in a non-standard location and cmake cannot
       find it, you will need to specify the path where cppunit's headers and
       library reside. Change the path for setting CppUnit_Inc_Dir to the
       include directory that contains the library's headers. This should be
       the path that contains the folder called 'cppunit'. Next, change the
       setting for CppUnit_Lib_Dir to the path where the binary shared library
       is located.

       Once you have confirmed all your settings, you need to press 'c' again,
       and then press 'g' to generate the actual makes files. This will return
       you back to the command line.

    4. After configuring the project using ccmake, build the project by typing: 
       
       make all;

    4a. If you have BUILD_UNITTESTS set to ON, you should have a unittest
        directory in your build directory. You can run unittests by running the
        executable: ./unittests/unittests 

    5. To install the project in the location of your CMAKE_INSTALL_PREFIX path
       type : 

       make install;

       Note that this may require you to have sudo/root privileges. Also, this
       will only install the library and its headers; it will not install
       unittests. 

    6. If you are having trouble building/installing the library using these
       steps, check the project website for documentation at
       http://www.nektarfx.com/nkhive. If you do not find your answers there,
       please contact us at code@nektarfx.com with a detailed account of the
       problem you are running into. 

--------------------------------------------------------------------------------

When Things Go Wrong

    If you are having problems building, installing, running, and/or using
    nkhive that you believe is attributed to a bug, please file a bug using the
    issue tracker on github at https://github.com/NektarFX/nkhive/issues or
    email us at code@nektarfx.com.

    In all bug-reports, *please* tell what version of the library you are
    talking about, how to duplicate the problem, and what your setup is. Please
    also check against existing outstanding issues on github to avoid filing
    duplicate bug reports. If you would like status on a particular issue/bug
    report, make a comment on the particular issue on github or email us at
    code@nektarfx.com, specifying which issue and project you are referring to.

    For bug reports, please try to build the library in debug mode and send us
    the debug dump information if there is any. 

--------------------------------------------------------------------------------
