set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" 
    ${CMAKE_SOURCE_DIR}/../../../../TMX-OAM/Build/Modules
    ${CMAKE_SOURCE_DIR}/../../../TMX-OAM/Build/Modules
    $ENV{HOME}/TMX-OAM/Build/Modules
    $ENV{TMX_OAM_HOME}/Build/Modules
)
cmake_minimum_required(VERSION 2.8.12)

project( TmxWebPortal NONE )

set(CMAKE_INSTALL_PREFIX /var/www)

set(CPACK_PACKAGE_NAME "tmx-webportal")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "The Battelle TMX core web portal")
set(CPACK_DEBIAN_PACKAGE_DEPENDS apache2 libapache2-mod-php5 
                                 phpmyadmin php-pear libpcre3-dev openssh-server 
                                 python python-setuptools websockify
                                 software-properties-common tmx-mysql)
set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA 
    ${CMAKE_CURRENT_SOURCE_DIR}/debian/postinst
    ${CMAKE_CURRENT_SOURCE_DIR}/debian/prerm)
include(TMX_Packaging)
include(CPack)

if(NOT MIPS_BUILD)
    install (DIRECTORY ${CMAKE_SOURCE_DIR}/. DESTINATION tmx
        PATTERN CMake* EXCLUDE
        PATTERN *Debug EXCLUDE
        PATTERN *Release EXCLUDE
        PATTERN conf EXCLUDE
        PATTERN debian EXCLUDE
    )
    
    if(EXISTS ${CMAKE_SOURCE_DIR}/conf/websockify.conf)
        install (FILES ${CMAKE_SOURCE_DIR}/conf/websockify.conf
                 DESTINATION ../../etc/init)
    endif()

    if(EXISTS ${CMAKE_SOURCE_DIR}/conf/default-site)
        install (FILES ${CMAKE_SOURCE_DIR}/conf/default-site
                 DESTINATION ../../etc/apache2/sites-available)
    endif()
endif(NOT MIPS_BUILD)
