set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" 
    ${CMAKE_SOURCE_DIR}/../../../../TMX-OAM/Build/Modules
    ${CMAKE_SOURCE_DIR}/../../../TMX-OAM/Build/Modules
    $ENV{HOME}/TMX-OAM/Build/Modules
    $ENV{TMX_OAM_HOME}/Build/Modules
)
cmake_minimum_required(VERSION 2.8.12)

project( tmxapi NONE )

set( CMAKE_C_DIALECT "-std=gnu89" )

set(CPACK_PACKAGE_NAME "lib${PROJECT_NAME}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "The Battelle TMX API Library")

# Build a dependency list for Boost
set(CPACK_DEBIAN_PACKAGE_DEPENDS libstdc++6 libasn_j2735_r41
                                 libboost-system1.55-dev libboost-thread1.55-dev
                                 libboost-atomic1.55-dev libboost-chrono1.55-dev
                                 libboost-regex1.55-dev libboost-log1.55-dev
                                 libboost-filesystem1.55-dev
                                 libboost-program-options1.55-dev)
include(TMX_C_Project)
include(CPack)

include(CheckCCompilerFlag)
check_c_compiler_flag("-fPIC" CMAKE_C_FLAG_FPIC_SUPPORTED)
if (CMAKE_C_FLAG_FPIC_SUPPORTED AND NOT CYGWIN)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
endif (CMAKE_C_FLAG_FPIC_SUPPORTED AND NOT CYGWIN)

file(GLOB_RECURSE SOURCES "tmx/*.c*")

find_package(TmxCommon COMPONENTS Asn_J2735 REQUIRED)
include_directories( ${Asn_J2735_INCLUDE_DIR} )

# Force static linking on MIPS builds
if(NOT MIPS_BUILD)
add_library (${PROJECT_NAME} SHARED ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${Asn_J2735_LIBRARY} ${Boost_LIBRARIES})
install (TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME} LIBRARY DESTINATION lib)
endif(NOT MIPS_BUILD)

add_library (${PROJECT_NAME}Static STATIC ${SOURCES})
set_target_properties(${PROJECT_NAME}Static PROPERTIES OUTPUT_NAME ${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME}Static ${Asn_J2735_LIBRARY} ${Boost_LIBRARIES})
install (TARGETS ${PROJECT_NAME}Static DESTINATION lib)


install (DIRECTORY tmx DESTINATION include FILES_MATCHING PATTERN "*.h*" PATTERN ".svn" EXCLUDE)

