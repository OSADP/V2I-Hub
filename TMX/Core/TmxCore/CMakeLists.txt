set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" 
    ${CMAKE_SOURCE_DIR}/../../../../TMX-OAM/Build/Modules
    ${CMAKE_SOURCE_DIR}/../../../TMX-OAM/Build/Modules
    $ENV{HOME}/TMX-OAM/Build/Modules
    $ENV{TMX_OAM_HOME}/Build/Modules
)
cmake_minimum_required(VERSION 2.8.12)

project( tmxcore NONE )

set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "The Battelle TMX core application")
set(CPACK_DEBIAN_PACKAGE_DEPENDS apache2 mysql-server libapache2-mod-php5 
                                 phpmyadmin php-pear libpcre3-dev 
                                 adduser openssh-server debconf 
                                 libsnmp30 libxerces-c3.1 libcurl3 
                                 python python-setuptools websockify
                                 software-properties-common libtmxapi)
set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/debian/postinst
    ${CMAKE_CURRENT_SOURCE_DIR}/src/debian/prerm
    ${CMAKE_CURRENT_SOURCE_DIR}/src/debian/templates)
include(TMX_C_Project)
include(CPack)

file(GLOB_RECURSE SOURCES "src/*.c*")

find_package ( TmxCommon COMPONENTS Asn_J2735 TmxApi REQUIRED )
find_package ( MySQL REQUIRED )
find_package ( JSONCPP REQUIRED )
find_package ( BoostProcess REQUIRED )

add_executable (${PROJECT_NAME} ${SOURCES} )

target_include_directories( ${PROJECT_NAME} PUBLIC ${Boost_INCLUDE_DIRS} ${MYSQL_INCLUDE_DIRS} ${MYSQLCPPCONN_INCLUDE_DIRS} ${JSONCPP_INCLUDE_DIRS} ${BoostProcess_INCLUDE_DIRS} )
target_link_libraries( ${PROJECT_NAME} pthread m rt ${TmxApi_LIBRARIES} ${Asn_J2735_LIBRARY} )
target_link_libraries( ${PROJECT_NAME} ${Boost_LIBRARIES} ${MYSQL_LIBRARIES} ${MYSQLCPPCONN_LIBRARIES} ${JSONCPP_LIBRARIES} )
if (ARM_BUILD)
    target_link_libraries( ${PROJECT_NAME} -lmysqlclient_r -lz )
endif()

install (TARGETS ${PROJECT_NAME} DESTINATION bin)
if(NOT MIPS_BUILD)
    # Package the MySQL C++ Connector since it conflicts with Boost 1.55
    # This should probably be removed once all the versions line up in a later Linux distribution
    if(ARM_BUILD)
        get_filename_component(MYSQLCPPCONN_LIBRARY_NAME ${MYSQLCPPCONN_LIBRARIES} NAME)
        install (FILES ${MYSQLCPPCONN_LIBRARIES} DESTINATION lib RENAME ${MYSQLCPPCONN_LIBRARY_NAME}.7 )
    else(ARM_BUILD)
        install (FILES ${MYSQLCPPCONN_LIBRARIES} DESTINATION lib)
        install (FILES ${MYSQLCPPCONN_LIBRARIES}.7 DESTINATION lib)
        install (FILES ${MYSQLCPPCONN_LIBRARIES}.7.1.1.3 DESTINATION lib)
    endif(ARM_BUILD)
    
    if(EXISTS ${CMAKE_SOURCE_DIR}/tmxcore.conf)
        install (FILES ${CMAKE_SOURCE_DIR}/tmxcore.conf
                 DESTINATION ../../etc/init)
    endif()
    
    if(EXISTS ${CMAKE_SOURCE_DIR}/websockify.conf)
        install (FILES ${CMAKE_SOURCE_DIR}/websockify.conf
                 DESTINATION ../../etc/init)
    endif()

    if(EXISTS ${CMAKE_SOURCE_DIR}/default-site)
        install (FILES ${CMAKE_SOURCE_DIR}/default-site
                 DESTINATION ../../etc/apache2/sites-available)
    endif()

    if(EXISTS ${CMAKE_SOURCE_DIR}/../TmxWebPortal)
        install (DIRECTORY ${CMAKE_SOURCE_DIR}/../TmxWebPortal/
                 DESTINATION ../../var/www/tmx)
    endif()
endif(NOT MIPS_BUILD)
