set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" 
    ${CMAKE_SOURCE_DIR}/../../../../TMX-OAM/Build/Modules
    ${CMAKE_SOURCE_DIR}/../../../TMX-OAM/Build/Modules
    $ENV{HOME}/TMX-OAM/Build/Modules
    $ENV{TMX_OAM_HOME}/Build/Modules
)
cmake_minimum_required(VERSION 2.8.12)

project( asn_j2735_r41 NONE )
set(PROJECT_VERSION 2.3.1)

set(CPACK_PACKAGE_NAME lib${PROJECT_NAME})

set(CMAKE_C_DIALECT " ")

set(CPACK_DEBIAN_PACKAGE_DEPENDS libc6)

include(TMX_C_Project)
include(CPack)

# Use these lines and rebuild for debug information
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DEMIT_ASN_DEBUG=1")
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DEMIT_ASN_DEBUG=1")

file(GLOB_RECURSE SOURCES "src/*.c*")

include_directories(BEFORE include/asn_j2735_r41)
add_library (${PROJECT_NAME} SHARED ${SOURCES})

install (TARGETS ${PROJECT_NAME} DESTINATION lib)
install (DIRECTORY include/asn_j2735_r41 DESTINATION include)
