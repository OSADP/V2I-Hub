# Top level cmake build file
# used to create an open-source distribution
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" ${CMAKE_SOURCE_DIR}/../../Modules)
cmake_minimum_required(VERSION 2.8.12)

# Only distribute the x86_64 release version
set(ARM_BUILD OFF)
set(MIPS_BUILD OFF)
set(CMAKE_BUILD_TYPE Release)

project(plugin-distributor NONE)

set(PROJECT_NAME tmx-plugins)
set(PROJECT_VERSION 2.2.0)

set(CPACK_SET_DESTDIR ON)
set(CMAKE_INSTALL_PREFIX "./tmx-plugins")

# Need to include some C flags and CPack definitions from C Module 
include(TMX_C_Project)

set(CPACK_GENERATOR "TGZ")
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR})

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Transportation Message eXchange (TMX) Common Plugin Source")

include(CPack)

configure_file(${CMAKE_SOURCE_DIR}/CMakeLists.in ${CMAKE_BINARY_DIR}/CMakeLists.txt-inst @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/CMakeLists.txt-inst DESTINATION . RENAME CMakeLists.txt)
install(FILES ${CMAKE_BINARY_DIR}/PluginDependency.cmake DESTINATION .)

set(USE_ivputils ON)

file(GLOB ALL_PLUGINS "${CMAKE_SOURCE_DIR}/../../../../../PluginsSrc/trunk/*")
foreach (plugindir ${ALL_PLUGINS})
    get_filename_component(pluginname ${plugindir} NAME)
    set(USE_PLUGIN ${USE_${pluginname}})
    if (USE_PLUGIN)
        message(STATUS "Including ${pluginname}")
        install(DIRECTORY ${plugindir}/src DESTINATION ${pluginname})
        if (EXISTS ${plugindir}/manifest.json)
            execute_process(COMMAND sed -e /\"exeLocation\":/s|\"/.*\"|\"/bin/${pluginname}\"|
                            ${plugindir}/manifest.json OUTPUT_FILE ${pluginname}-manifest.json)
            install(FILES ${pluginname}-manifest.json DESTINATION ${pluginname} RENAME manifest.json)
        endif (EXISTS ${plugindir}/manifest.json)
        
        if (EXISTS ${pluginname}-CMakeLists.in)
            configure_file(${pluginname}-CMakeLists.in ${pluginname}-CMakeLists-inst.txt @ONLY)
        else (EXISTS ${pluginname}-CMakeLists.in)
            set(PLUGIN ${pluginname})
            
            file(STRINGS ${plugindir}/CMakeLists.txt REQUIRE_XERCES_C REGEX "target_link_libraries.*XercesC_LIBRAR")
            if (REQUIRE_XERCES_C)
                set(REQUIRE_XERCES_C ON)
            else (REQUIRE_XERCES_C)
                set(REQUIRE_XERCES_C OFF)
            endif (REQUIRE_XERCES_C)

            file(STRINGS ${plugindir}/CMakeLists.txt REQUIRE_SNMP REGEX "target_link_libraries.*NETSNMP_LIBRAR")
            if (REQUIRE_SNMP)
                set(REQUIRE_SNMP ON)
            else (REQUIRE_SNMP)
                set(REQUIRE_SNMP OFF)
            endif (REQUIRE_SNMP)

            file(STRINGS ${plugindir}/CMakeLists.txt REQUIRE_CURL REGEX "target_link_libraries.*CURL_LIBRAR")
            if (REQUIRE_CURL)
                set(REQUIRE_CURL ON)
            else (REQUIRE_CURL)
                set(REQUIRE_CURL OFF)
            endif (REQUIRE_CURL)
            
            configure_file(plugin-CMakeLists.in ${pluginname}-CMakeLists-inst.txt @ONLY)
        endif (EXISTS ${pluginname}-CMakeLists.in)
        install(FILES ${CMAKE_BINARY_DIR}/${pluginname}-CMakeLists-inst.txt DESTINATION ${pluginname} RENAME CMakeLists.txt)
    else ()
        message(STATUS "Skipping ${pluginname}")
    endif ()
endforeach ()
