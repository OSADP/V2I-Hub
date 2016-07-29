set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" 
    ${CMAKE_SOURCE_DIR}/../../../../TMX-OAM/Build/Modules
    ${CMAKE_SOURCE_DIR}/../../../TMX-OAM/Build/Modules
    $ENV{HOME}/TMX-OAM/Build/Modules
    $ENV{TMX_OAM_HOME}/Build/Modules
)
cmake_minimum_required(VERSION 2.8.12)

project( SpatPlugin NONE )

set ( XTRA_BOOST_LIBRARIES date_time )

include(TMX_Plugin_Project)

find_package ( XercesC REQUIRED )
find_package ( NetSNMP REQUIRED )

target_include_directories( ${PROJECT_NAME} PUBLIC ${Boost_INCLUDE_DIRS} ${XercesC_INCLUDE_DIRS} ${NETSNMP_INCLUDE_DIRS} )
target_link_libraries( ${PROJECT_NAME} rt ${Boost_LIBRARIES} ${XercesC_LIBRARIES} ${NETSNMP_LIBRARIES} )
