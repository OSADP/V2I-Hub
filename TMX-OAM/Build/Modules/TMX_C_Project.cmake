# Global values
set(CMAKE_VERBOSE_MAKEFILE ON CACHE INTERNAL "C Project setting")
set(CMAKE_USE_RELATIVE_PATHS ON CACHE INTERNAL "C Project setting")

# Subversion needs to be installed 
find_package(Subversion 1.8)

# Determine the root path for both the source directory and the build script directory
get_filename_component(TMX_BUILD_DIR ${CMAKE_CURRENT_LIST_DIR} DIRECTORY CACHE)
get_filename_component(TMX_OAM_DIR ${TMX_BUILD_DIR} DIRECTORY CACHE)
get_filename_component(CMAKE_SOURCE_COMPONENT_NAME ${CMAKE_SOURCE_DIR} NAME CACHE)

# Retrieve the subversion repo information
Subversion_WC_INFO(${TMX_OAM_DIR} TMX_OAM)
Subversion_WC_INFO(${CMAKE_SOURCE_DIR}/.. SOURCE_DIR)

# Just a quick check to ensure both are from the same Subversion source
if (NOT TMX_OAM_WC_ROOT STREQUAL SOURCE_DIR_WC_ROOT)
    message(WARNING "Subversion root ${TMX_OAM_WC_URL} is not the same source as ${SOURCE_DIR_WC_URL}.")
endif()

# Find the local directory of the repository for use later
string(REGEX REPLACE "^${TMX_OAM_WC_ROOT}" "" TMX_OAM_WC_SUBPATH "${TMX_OAM_WC_URL}")
string(REGEX REPLACE "${TMX_OAM_WC_SUBPATH}$" "" TMX_OAM_WC_LOCALDIR "${TMX_OAM_DIR}")
string(REGEX REPLACE "^${TMX_OAM_WC_ROOT}" "" SOURCE_DIR_WC_SUBPATH "${SOURCE_DIR_WC_URL}")
string(REGEX REPLACE "${SOURCE_DIR_WC_SUBPATH}/${CMAKE_SOURCE_COMPONENT_NAME}$" "" SOURCE_DIR_WC_LOCALDIR "${CMAKE_SOURCE_DIR}")

# Try to determine the config type from the working directory
get_filename_component(PROJECT_WORKING_DIR $ENV{PWD} ABSOLUTE)
get_filename_component(PROJECT_PARENT_DIR ${PROJECT_WORKING_DIR} DIRECTORY)

# Assume the source is a level up, and if so, use the name of the directory
# as the config.  Otherwise, rely on the build type parameters
if(${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${PROJECT_PARENT_DIR})
    get_filename_component(PROJECT_BUILD_TYPE_DIR ${PROJECT_WORKING_DIR} NAME)
elseif(DEFINED CMAKE_BINARY_DIR)
    get_filename_component(PROJECT_BUILD_TYPE_DIR ${CMAKE_BINARY_DIR} NAME)
endif()

if(PROJECT_BUILD_TYPE_DIR MATCHES "MIPS-.*$")
    set(MIPS_BUILD ON)
else()
    set(MIPS_BUILD OFF)
endif()

if(PROJECT_BUILD_TYPE_DIR MATCHES "ARM-.*$")
    set(ARM_BUILD ON)
else()
    set(ARM_BUILD OFF)
endif()

# Assume trunk is the branch
if (NOT DEFINED TMX_BRANCH)
  set(TMX_BRANCH trunk)
endif (NOT DEFINED TMX_BRANCH)

# Find the Libraries directory
if (NOT DEFINED TMX_LIBRARIES_DIR)
    if (EXISTS ${TMX_OAM_DIR}/Libraries)
        set(TMX_LIBRARIES_DIR ${TMX_OAM_DIR}/Libraries)
    endif()
endif (NOT DEFINED TMX_LIBRARIES_DIR)

if (NOT DEFINED TMX_CORE_DIR)
    # Try to search for the Core components within the current
    # directory structure.  Otherwise, the libraries must be installed
    #
    # First. see if it is within the source path
    if (EXISTS ${CMAKE_SOURCE_DIR}/../../Core)
        set(TMX_CORE_DIR ${CMAKE_SOURCE_DIR}/../../Core)
    # Or, check at the root of the tree
    elseif (EXISTS ${SOURCE_DIR_WC_LOCALDIR}/TMX/Core)
        set(TMX_CORE_DIR ${SOURCE_DIR_WC_LOCALDIR}/TMX/Core)
    elseif (EXISTS ${TMX_OAM_WC_LOCALDIR}/TMX/Core)
        set(TMX_CORE_DIR ${TMX_OAM_WC_LOCALDIR}/TMX/Core)
    elseif (EXISTS ${SOURCE_DIR_WC_LOCALDIR}/TMX/${TMX_BRANCH}/Core)
        set(TMX_CORE_DIR ${SOURCE_DIR_WC_LOCALDIR}/TMX/${TMX_BRANCH}/Core)
    elseif (EXISTS ${TMX_OAM_WC_LOCALDIR}/TMX/${TMX_BRANCH}/Core)
        set(TMX_CORE_DIR ${TMX_OAM_WC_LOCALDIR}/TMX/${TMX_BRANCH}/Core)
    endif()
endif (NOT DEFINED TMX_CORE_DIR)

if (NOT IS_DIRECTORY ${TMX_LIBRARIES_DIR})
    message(WARNING "Unable to locate the Libraries sub-directory.")
endif()
    
# Done before the language is enabled so the compiler test is picked up
if(MIPS_BUILD)
    message(STATUS "Using Arada MIPS Cross Compiler")
    
    if(NOT MIPS_TOOLCHAIN)
        set(MIPS_TOOLCHAIN "2.0")
    endif(NOT MIPS_TOOLCHAIN)
    
    if(MIPS_TOOLCHAIN EQUAL 2.0)
        set(MIPS_LIB_VERSION 2.0.0.7)
        set(CMAKE_C_COMPILER "/opt/buildroot-2011.08-rc1/output/host/usr/bin/mips-linux-gcc")
        set(CMAKE_CXX_COMPILER "/opt/buildroot-2011.08-rc1/output/host/usr/bin/mips-linux-g++")
	if (NOT DEFINED CMAKE_C_DIALECT)
            set(CMAKE_C_DIALECT "-std=gnu89")
	endif (NOT DEFINED CMAKE_C_DIALECT)
	if (NOT DEFINED CMAKE_CXX_DIALECT)
            set(CMAKE_CXX_DIALECT "-std=gnu++98")
	endif (NOT DEFINED CMAKE_CXX_DIALECT)
    elseif(MIPS_TOOLCHAIN EQUAL 1.42)
        set(MIPS_LIB_VERSION 1.86.4)
        set(CMAKE_C_COMPILER "/opt/build/toolchain/PB44/buildroot/build_mips/staging_dir/usr/bin/mips-linux-uclibc-gcc")
        set(CMAKE_CXX_COMPILER "/opt/build/toolchain/PB44/buildroot/build_mips/staging_dir/usr/bin/mips-linux-uclibc-g++")
    endif(MIPS_TOOLCHAIN EQUAL 2.0)

    set(MIPS_TOOLCHAIN_DIR "${TMX_LIBRARIES_DIR}/locomate-mips_${MIPS_LIB_VERSION}")
    link_directories(${MIPS_TOOLCHAIN_DIR}/lib)
endif(MIPS_BUILD)

if(ARM_BUILD)
    message(STATUS "Using ARM Cross Compiler")
    if (EXISTS /usr/bin/arm-linux-gnueabihf-gcc-4.8)
        set(CMAKE_C_COMPILER "/usr/bin/arm-linux-gnueabihf-gcc-4.8")
    elseif (EXISTS /usr/bin/arm-linux-gnueabihf-gcc-4.7)
        message(WARNING "ARM Cross Compiler is GCC 4.7.  Please upgrade to GCC 4.8 soon.")
        set(CMAKE_C_COMPILER "/usr/bin/arm-linux-gnueabihf-gcc-4.7")
    else ()
        message(FATAL_ERROR "No ARM C Cross Compiler installed.  Please apt-get install them.")
    endif ()
    
    if (EXISTS /usr/bin/arm-linux-gnueabihf-g++-4.8)
        set(CMAKE_CXX_COMPILER "/usr/bin/arm-linux-gnueabihf-g++-4.8")
    elseif (EXISTS /usr/bin/arm-linux-gnueabihf-g++-4.7)
        message(WARNING "ARM Cross Compiler is G++ 4.7.  Please upgrade to G++ 4.8 soon.")
        set(CMAKE_CXX_COMPILER "/usr/bin/arm-linux-gnueabihf-g++-4.7")
    else()
        message(FATAL_ERROR "No ARM C++ Cross Compiler installed.  Please apt-get install them.")
    endif()
    
    set(ARM_TOOLCHAIN_DIR "${TMX_LIBRARIES_DIR}/arm")
    link_directories(${ARM_TOOLCHAIN_DIR}/lib)
endif(ARM_BUILD)

# Compiler Flags
enable_language(C CXX)

if(NOT CMAKE_C_OPTLEVEL)
    set(CMAKE_C_OPTLEVEL 0)
endif(NOT CMAKE_C_OPTLEVEL)
if(NOT CMAKE_CXX_OPTLEVEL)
    set(CMAKE_CXX_OPTLEVEL 0)
endif(NOT CMAKE_CXX_OPTLEVEL)

if(NOT CMAKE_C_DIALECT)
    set(CMAKE_C_DIALECT "-std=c99")
endif(NOT CMAKE_C_DIALECT)
if(NOT CMAKE_CXX_DIALECT AND NOT MIPS_BUILD)
    set(CMAKE_CXX_DIALECT "-std=c++0x")
endif(NOT CMAKE_CXX_DIALECT AND NOT MIPS_BUILD)

set(CMAKE_C_FLAGS_DEBUG "-g3")
set(CMAKE_CXX_FLAGS_DEBUG "-g3")

set(CMAKE_C_FLAGS "${CMAKE_C_DIALECT} -O${CMAKE_C_OPTLEVEL} -Wall -fmessage-length=0 ${CMAKE_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_DIALECT} -O${CMAKE_CXX_OPTLEVEL} -Wall -fmessage-length=0 ${CMAKE_CXX_FLAGS}")

# The ARM compiler is gcc 4.7 based, which has a bug in this_thread::sleep_for and the like
if(ARM_BUILD)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_GLIBCXX_USE_NANOSLEEP=1")
endif()

# Linker options
set(CMAKE_EXE_LINKER_FLAGS -Wl,--allow-shlib-undefined)

# Get build config information
if(DEFINED PROJECT_BUILD_TYPE_DIR)
    set(CMAKE_BINARY_DIR "${CMAKE_SOURCE_DIR}/${PROJECT_BUILD_TYPE_DIR}")

    string(REGEX MATCH ".*(Debug|Release)" CMAKE_BUILD_TYPE_MATCH ${PROJECT_BUILD_TYPE_DIR})
    if(CMAKE_BUILD_TYPE_MATCH)
        set(CMAKE_BUILD_TYPE ${CMAKE_MATCH_1})
    else()
        message(WARNING "Unknown C/C++ build config found from directory ${PROJECT_BUILD_TYPE_DIR}.  Default is Debug")
    endif()
endif(DEFINED PROJECT_BUILD_TYPE_DIR)

if(NOT DEFINED CMAKE_BUILD_TYPE OR NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif(NOT DEFINED CMAKE_BUILD_TYPE OR NOT CMAKE_BUILD_TYPE)

if(NOT DEFINED CMAKE_BINARY_DIR)
    if(MIPS_BUILD)
        set(CMAKE_BINARY_DIR_PREFIX "MIPS-")
    elseif(ARM_BUILD)
        set(CMAKE_BINARY_DIR_PREFIX "ARM-")
    else(MIPS_BUILD)
        set(CMAKE_BINARY_DIR_PREFIX "")
    endif(MIPS_BUILD)
    set(CMAKE_BINARY_DIR 
        "${CMAKE_SOURCE_DIR}/${CMAKE_BINARY_DIR_PREFIX}${CMAKE_BUILD_TYPE}")
endif(NOT DEFINED CMAKE_BINARY_DIR)

# Add Debug flag
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DTMX_DEBUG")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DTMX_DEBUG")
endif()

# Link options
set(CMAKE_SKIP_BUILD_RPATH FALSE)

# For Debug, use the local build paths
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
else()
    set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE)
endif()

set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
if(MIPS_BUILD)
    set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_RPATH};/var/lib")
endif()


# Platform identification
cmake_host_system_information(RESULT PROJECT_HOSTNAME QUERY HOSTNAME)
message(STATUS "Build initialized on host ${PROJECT_HOSTNAME}")
if(WIN32)
    message(FATAL_ERROR "Must build ${PROJECT_NAME} ${CMAKE_BUILD_TYPE} config on Windows under Cygwin!")
endif(WIN32)
if(APPLE)
    message(FATAL_ERROR "Unable to build ${PROJECT_NAME} ${CMAKE_BUILD_TYPE} config on Mac OS!")
endif(APPLE)
if (UNIX)
    if(CYGWIN)
        message(STATUS "Building C Project ${PROJECT_NAME} ${CMAKE_BUILD_TYPE} config on Windows using Cygwin")
    else(CYGWIN)
        message(STATUS "Building C Project ${PROJECT_NAME} ${CMAKE_BUILD_TYPE} config on Unix/Linux")
    endif(CYGWIN)
endif(UNIX)

# Add include libraries for Cygwin
if (CYGWIN)
    include_directories(${TMX_LIBRARIES_DIR}/cygwin)
endif(CYGWIN)

# All C++ projects will use Boost, so this provides the standard configuration
if (NOT NO_BOOST)
    # General features enabled
    add_definitions(-DBOOST_ASIO_HEADER_ONLY)
    set(Boost_USE_MULTITHREADED ON)
        
    # For Arada, the Boost headers and libraries are under the toolchain directory 
    if (MIPS_BUILD)
        set(BOOST_LIBRARYDIR "${MIPS_TOOLCHAIN_DIR}/lib")    
        include_directories("${MIPS_TOOLCHAIN_DIR}/incs")
    
        # Some specific features need enabled or disabled
        add_definitions(-DBOOST_NO_INTRINSIC_WCHAR_T -DBOOST_ASIO_DISABLE_EPOLL -DBOOST_ASIO_DISABLE_FENCED_BLOCK -DBOOST_AC_USE_PTHREADS)
        set(Boost_USE_STATIC_RUNTIME ON)
    elseif(ARM_BUILD)
	    set(BOOST_LIBRARYDIR "${ARM_TOOLCHAIN_DIR}/lib")
	    include_directories("${ARM_TOOLCHAIN_DIR}/incs")

        # Some specific features need enabled or disabled
        add_definitions (-DMX_NO_BT -DBOOST_ALL_DYN_LINK)
        add_definitions (-DBOOST_AC_USE_PTHREADS)
        set(Boost_USE_STATIC_RUNTIME OFF)
    else(MIPS_BUILD)
        add_definitions (-DMX_NO_BT -DBOOST_ALL_DYN_LINK)
        set(Boost_USE_STATIC_RUNTIME OFF)
    endif(MIPS_BUILD)

    # Find the libraries and configure the build path
    find_package(Boost 1.55 COMPONENTS system thread atomic chrono regex log log_setup filesystem program_options ${XTRA_BOOST_LIBRARIES} REQUIRED)
    include_directories(${Boost_INCLUDE_DIRS})
    link_directories(${Boost_LIBRARY_DIRS})
endif (NOT NO_BOOST)

# Common Packaging configuration.  The include(CPack) must be done after this include
set(CPACK_GENERATOR "TGZ")                            # Build GZip Tarbal distribution
if(NOT MIPS_BUILD AND NOT CYGWIN)
    set(CPACK_GENERATOR "${CPACK_GENERATOR};DEB")     # Build .deb package distribution also
endif(NOT MIPS_BUILD AND NOT CYGWIN)
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY "OFF")           # Do not include a sub-directory for the project
set(CPACK_PACKAGE_VENDOR "battelle.org")              # A Battelle product
set(CPACK_PACKAGE_CONTACT "baumgardner@battelle.org") # Build Administrator

# The Debian dependencies should be comma separated not semi-colon
if(DEFINED CPACK_DEBIAN_PACKAGE_DEPENDS)
    string(REGEX REPLACE ";" ", " CPACK_DEBIAN_PACKAGE_DEPENDS "${CPACK_DEBIAN_PACKAGE_DEPENDS}")
endif()

# Installation defaults
set(CPACK_PACKAGE_INSTALL_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CPACK_PACKAGING_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

if(NOT DEFINED CPACK_PACKAGE_NAME)
    set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
endif()

# Package versioning
if(NOT DEFINED PROJECT_VERSION)
    # Search for the version header
    find_file(PROJECT_VERSION_H version.h HINTS 
                ${CMAKE_SOURCE_DIR}/src 
                ${CMAKE_SOURCE_DIR}/tmx 
                ${CMAKE_SOURCE_DIR}/source 
                NO_DEFAULT_PATH)
    if(PROJECT_VERSION_H)
        file(STRINGS ${PROJECT_VERSION_H} PROJECT_VERSION REGEX "#define.*([0-9\\.]+)")
        string(REGEX MATCH "([0-9\\.]+)" PROJECT_VERSION "${PROJECT_VERSION}")
    else(PROJECT_VERSION_H)
        set(PROJECT_VERSION 0.0.1)
    endif(PROJECT_VERSION_H)
endif()

if(NOT DEFINED PROJECT_VERSION_MAJOR)
    string(REGEX MATCH "([0-9]+)\\.?([0-9]+)?\\.?([0-9]+)?" PROJECT_VERSION_MATCH ${PROJECT_VERSION} )
    if(DEFINED CMAKE_MATCH_1)
        set(PROJECT_VERSION_MAJOR ${CMAKE_MATCH_1})
    endif()
    if(DEFINED CMAKE_MATCH_2)
        set(PROJECT_VERSION_MINOR ${CMAKE_MATCH_2})
    endif()
    if(DEFINED CMAKE_MATCH_3)
        set(PROJECT_VERSION_PATCH ${CMAKE_MATCH_3})
    endif()
endif()

set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})

# An additional build version suffix may be injected into the environment
if(DEFINED ENV{CPACK_VERSION_SUFFIX})
    set(CPACK_PACKAGE_VERSION_PATCH "${CPACK_PACKAGE_VERSION_PATCH}-$ENV{CPACK_VERSION_SUFFIX}")
endif()
set(CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")

if(MIPS_BUILD)
    set(CPACK_LIBRARY_ARCHITECTURE "arada-mips")
elseif(ARM_BUILD)
    set(CPACK_LIBRARY_ARCHITECTURE "armhf")
else(MIPS_BUILD)
    set(CPACK_LIBRARY_ARCHITECTURE "amd64") 
endif(MIPS_BUILD)

# Some specific Debian package fields
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE ${CPACK_LIBRARY_ARCHITECTURE})

# Custom file name
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_LIBRARY_ARCHITECTURE}")

# Test infrastructure
file(GLOB_RECURSE ${PROJECT_NAME}TEST-SOURCES "${CMAKE_SOURCE_DIR}/test/*.c*" "${CTEST_TEST_SOURCES}")
if (${PROJECT_NAME}TEST-SOURCES)
    enable_testing()

    set(CTEST_TEST_NAME test-${CPACK_PACKAGE_NAME})
    
    add_executable(${CTEST_TEST_NAME} EXCLUDE_FROM_ALL ${${PROJECT_NAME}TEST-SOURCES})

    # Add the Google test framework linkage
    set(GTEST_DIR ${TMX_LIBRARIES_DIR}/googletest)
    target_include_directories(${CTEST_TEST_NAME} PUBLIC ${GTEST_DIR}/include)
    target_link_libraries(${CTEST_TEST_NAME} 
        ${GTEST_DIR}/lib/libgtest.a ${GTEST_DIR}/lib/libgtest_main.a
        ${GTEST_DIR}/lib/libgmock.a ${GTEST_DIR}/lib/libgmock_main.a
        pthread)
   
   # Create a test to run
   add_test(make_${PROJECT_NAME}test "${CMAKE_COMMAND}" --build "${CMAKE_BINARY_DIR}" --target ${CTEST_TEST_NAME})
   add_test(${PROJECT_NAME}test ${CTEST_TEST_NAME} CONFIGURATIONS ${CMAKE_BUILD_TYPE})
   set_tests_properties(${PROJECT_NAME}test PROPERTIES DEPENDS make_${PROJECT_NAME}test)
   
   add_custom_target(test-results cat "${CMAKE_BINARY_DIR}/Testing/Temporary/LastTest.log")
endif (${PROJECT_NAME}TEST-SOURCES)
