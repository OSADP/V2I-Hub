set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CMAKE_INSTALL_PREFIX "/")

# Read the version info from the manifest file
file(STRINGS ${CMAKE_SOURCE_DIR}/manifest.json MANIFEST_DATA REGEX "\"version\":")
string(REGEX MATCH "([0-9\\.]+)" PROJECT_VERSION "${MANIFEST_DATA}")

include(TMX_C_Project)

# Plugins ZIP the executable along with the manifest files, but can also be installed directly
# via apt-get on x86 and ARM
set(CPACK_GENERATOR "ZIP")
if (NOT MIPS_BUILD)
    set(CPACK_GENERATOR "${CPACK_GENERATOR};DEB")
    
    # Use lowercase 
    string(TOLOWER "tmx-${CPACK_PACKAGE_NAME}" CPACK_DEBIAN_PACKAGE_NAME)
        
    # Set Description from manifest
    file(STRINGS ${CMAKE_SOURCE_DIR}/manifest.json MANIFEST_DESCR_DATA REGEX "\"description\":")
    string(REGEX MATCH ":\"([^\"]*)\"," MANIFEST_DESCR "${MANIFEST_DESCR_DATA}")
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${CMAKE_MATCH_1})
    
    # Minimum dependency on tmxapi, not on tmxcore because some plugins install externally
    set(CPACK_DEBIAN_PACKAGE_DEPENDS libtmxapi)
    
    # Create a config file that alters the install path for DEB vs. ZIP
    set(CPACK_SET_DESTDIR "OFF")
    
    # The install directory for DEB is different than for ZIP files
    # since the Debian package must extract to the full path location
    file(STRINGS ${CMAKE_SOURCE_DIR}/manifest.json MANIFEST_NAME_DATA REGEX "\"name\":")
    string(REGEX MATCH ":\"([^\"]*)\"," MANIFEST_NAME "${MANIFEST_NAME_DATA}")
    string(REGEX REPLACE " " "" CPACK_DEBIAN_PACKAGE_INSTALL_PREFIX "/var/www/plugins/${CMAKE_MATCH_1}")
    set(CPACK_PROJECT_CONFIG_FILE "${TMX_OAM_DIR}/Build/packaging/CPackPluginConfig.cmake.in")
    
    configure_file(${TMX_OAM_DIR}/Build/packaging/debian/postinst ${CMAKE_BINARY_DIR}/postinst @ONLY)
    configure_file(${TMX_OAM_DIR}/Build/packaging/debian/prerm ${CMAKE_BINARY_DIR}/prerm @ONLY)
    set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA 
        ${CMAKE_BINARY_DIR}/postinst
        ${CMAKE_BINARY_DIR}/prerm)
endif (NOT MIPS_BUILD)

include(CPack)

# Note that some of these dependencies may not be built yet, so they are not listed as required
find_package(TmxCommon COMPONENTS Asn_J2735 TmxApi)

# MIPS builds should not link the ASN library since it does not 
# seem to include UPER encoding correctly
if(MIPS_BUILD)
    find_package(Arada REQUIRED)
else(MIPS_BUILD)
    find_package(TmxCommon COMPONENTS TmxUtils)
endif(MIPS_BUILD)

file(GLOB_RECURSE SOURCES "src/*.c*")

add_executable (${PROJECT_NAME} ${SOURCES} )

install(TARGETS ${PROJECT_NAME} DESTINATION bin )

# Need to substitute the path to the file in the manifest
set(CMAKE_BINARY_TARGET_DIRECTORY "/bin")
if(POLICY CMP0026)
    cmake_policy(SET CMP0026 OLD)
endif(POLICY CMP0026)
get_target_property(CMAKE_BINARY_TARGET_OUTPUT_ABSOLUTE ${PROJECT_NAME} LOCATION)
get_filename_component(CMAKE_BINARY_TARGET_OUTPUT_NAME ${CMAKE_BINARY_TARGET_OUTPUT_ABSOLUTE} NAME)
set(CMAKE_BINARY_TARGET_OUTPUT_PATH "${CMAKE_BINARY_TARGET_DIRECTORY}/${CMAKE_BINARY_TARGET_OUTPUT_NAME}")

set(CPACK_MANIFEST_UPDATE_COMMAND "execute_process(COMMAND sed -e /\"exeLocation\":/s|\"/.*\"|")
set(CPACK_MANIFEST_UPDATE_COMMAND "${CPACK_MANIFEST_UPDATE_COMMAND}\"${CMAKE_BINARY_TARGET_OUTPUT_PATH}\"|")
set(CPACK_MANIFEST_UPDATE_COMMAND "${CPACK_MANIFEST_UPDATE_COMMAND} ${CMAKE_SOURCE_DIR}/manifest.json")
set(CPACK_MANIFEST_UPDATE_COMMAND "${CPACK_MANIFEST_UPDATE_COMMAND} OUTPUT_FILE ")
set(CPACK_MANIFEST_UPDATE_COMMAND "${CPACK_MANIFEST_UPDATE_COMMAND} ${CMAKE_BINARY_DIR}/manifest.json)")
install(CODE "${CPACK_MANIFEST_UPDATE_COMMAND}")
install(FILES ${CMAKE_BINARY_DIR}/manifest.json DESTINATION . )

# Pick up any XML config files for the plugin
file(GLOB CONFIG_FILES "*.xml")
if(CONFIG_FILES)
    install(FILES ${CONFIG_FILES} DESTINATION .)
endif(CONFIG_FILES)

if (NOT PLUGIN_NO_LINKS)
    if (ARM_BUILD)
        find_library(UUID_LIBRARY libuuid.so.1.3.0 HINTS ${ARM_TOOLCHAIN_DIR}/lib) 
    elseif(NOT MIPS_BUILD)
        find_library(UUID_LIBRARY libuuid.so.1.3.0)
    endif(ARM_BUILD)
    
    if(MIPS_BUILD)
        target_link_libraries( ${PROJECT_NAME} ${ARADA_LIBRARIES} ${TmxApi_LIBRARIES} ${Boost_LIBRARIES} )
    else(MIPS_BUILD)
        target_link_libraries( ${PROJECT_NAME} ${Asn_J2735_LIBRARY} ${TmxUtils_LIBRARIES} ${TmxApi_LIBRARIES} ${Boost_LIBRARIES} ${UUID_LIBRARY} )
    endif(MIPS_BUILD)
    target_link_libraries( ${PROJECT_NAME} pthread m )
endif (NOT PLUGIN_NO_LINKS)
