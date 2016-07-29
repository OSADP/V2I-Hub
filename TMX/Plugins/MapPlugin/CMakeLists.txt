set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" 
    ${CMAKE_SOURCE_DIR}/../../../../TMX-OAM/Build/Modules
    ${CMAKE_SOURCE_DIR}/../../../TMX-OAM/Build/Modules
    $ENV{HOME}/TMX-OAM/Build/Modules
    $ENV{TMX_OAM_HOME}/Build/Modules
)
cmake_minimum_required(VERSION 2.8.12)

project( MapPlugin NONE )

include(TMX_Plugin_Project)

find_package(XercesC REQUIRED)

target_include_directories( ${PROJECT_NAME} PUBLIC ${XercesC_INCLUDE_DIRS} )
target_link_libraries( ${PROJECT_NAME} rt ${XercesC_LIBRARY} )
