set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" 
    ${CMAKE_SOURCE_DIR}/../../../../TMX-OAM/Build/Modules
    ${CMAKE_SOURCE_DIR}/../../../TMX-OAM/Build/Modules
    $ENV{HOME}/TMX-OAM/Build/Modules
    $ENV{TMX_OAM_HOME}/Build/Modules
)
cmake_minimum_required(VERSION 2.8.12)

project( RtcmPlugin NONE )

include(TMX_Plugin_Project)

if (ARM_BUILD)
  set (CURL_INCLUDE_DIRS ${ARM_TOOLCHAIN_DIR}/incs)
  set (CURL_LIBRARIES -lcurl)
else (ARM_BUILD)
  find_package(CURL REQUIRED)
endif (ARM_BUILD)

target_include_directories( ${PROJECT_NAME} PUBLIC ${CURL_INCLUDE_DIRS} )
target_link_libraries( ${PROJECT_NAME} ${CURL_LIBRARIES} )
