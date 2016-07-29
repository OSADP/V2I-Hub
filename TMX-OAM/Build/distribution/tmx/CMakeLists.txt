# Top level cmake build file
# used to create an open-source distribution
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" ${CMAKE_SOURCE_DIR}/../../Modules)
cmake_minimum_required(VERSION 2.8.12)

# Only distribute the x86_64 release version
set(ARM_BUILD OFF)
set(MIPS_BUILD OFF)
set(CMAKE_BUILD_TYPE Release)

project(tmx-distributor NONE)

set(PROJECT_NAME tmx)
set(PROJECT_VERSION 2.3.0)

set(CMAKE_C_DIALECT " ")
set(CPACK_SET_DESTDIR ON)
set(CMAKE_INSTALL_PREFIX "./tmx")

# Need to include some C flags and CPack definitions from C Module 
include(TMX_C_Project)

set(CPACK_GENERATOR "TGZ")
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR})

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Transportation Message eXchange (TMX) Core Library Source")

include(CPack)

configure_file(${CMAKE_SOURCE_DIR}/CMakeLists.in ${CMAKE_BINARY_DIR}/CMakeLists.txt-inst @ONLY)

install(FILES ${CMAKE_BINARY_DIR}/CMakeLists.txt-inst DESTINATION . RENAME CMakeLists.txt)
install(DIRECTORY ${TMX_CORE_DIR}/Asn_J2735/include DESTINATION ./Asn_J2735) 
install(DIRECTORY ${TMX_CORE_DIR}/Asn_J2735/src DESTINATION ./Asn_J2735)
install(DIRECTORY ${TMX_CORE_DIR}/TmxApi/tmx DESTINATION ./TmxApi)
install(DIRECTORY ${TMX_CORE_DIR}/TmxCore/src DESTINATION ./TmxCore
        PATTERN "src/debian" EXCLUDE)
install(DIRECTORY ${TMX_LIBRARIES_DIR}/boost DESTINATION TmxCore/src)
