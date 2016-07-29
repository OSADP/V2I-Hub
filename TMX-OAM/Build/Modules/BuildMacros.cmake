# Top level cmake build file
# used to construct the Makefiles
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" ${CMAKE_SOURCE_DIR}/Build/Modules)
cmake_minimum_required(VERSION 2.8.12)

project(make-maker NONE)

# Targets created include:
#    cleaner                  - Cleans all the build artifacts.
#    cleanall                 - Cleans all the build artifacts plus all the CMake generated files
#    ls                       - Lists the build artifact output directory
#    cmake                    - Runs cmake on each of the build artifact output directories
#    makeall                  - Runs make all on each of the build artifact output directories
#    depends                  - Runs make depend on each of the build artifact output directories
#    install                  - Runs make install on each of the build artifact output directories
#    list_install_components  - Runs make list_install_components on each of the build artifact output directories
#    package                  - Runs make package on each of the build artifact output directories
#    package_source           - Runs make package_source on each of the build artifact output directories
#    rebuild_cache            - Runs make rebuild_cache on each of the build artifact output directories
#    all                      - Defaults to cmake and makeall
#
# Note that the cleaner target invokes the clean target for the Makefiles in the build artifact output directories.
#
# Additionally, each of the targets will have a per-config 
# i.e. (Debug, Release, ARM-Debug, ARM-Release, MIPS-Debug, MIPS-Release)
# target, as well as a specific version for each component and a per-config for each component.
#
# So, for example, the target "cmake_Debug" runs the cmake target only on the Debug config
# And the target "cmake_TmxCore" runs the cmake target only on the TmxCore config
# You can further restrict by using the target "cmake_TmxCore_Debug" to run the cmake target for TmxCore Debug config
#

# Set some defaults to unspecified arguments
if(NOT DEFINED CONFIG_DIRS)
    set(CONFIG_DIRS Debug Release ARM-Debug ARM-Release MIPS-Debug MIPS-Release)
endif()

if (NOT DEFINED TMX_BRANCH)
    set(TMX_BRANCH trunk)
endif (NOT DEFINED TMX_BRANCH)

set(TARGETS clean cleanall ls cmake makeall depends install list_install_components package package_source rebuild_cache)
macro(build_targets TARGET_SUFFIX BUILD_COMMAND)
    foreach(TARGET ${TARGETS})
        set(THIS_TARGET ${TARGET}${TARGET_SUFFIX})
        if(TARGET ${THIS_TARGET})
            # Cannot duplicate the target
            break()
        endif()

        if(${TARGET} STREQUAL "makeall")
            set(TARGET "all")
        endif()
 
        if(${BUILD_COMMAND})
             if(${ARGC} LESS 3)
                message(FATAL_ERROR "Need a directory to build the command to")
            endif()
            
            if(THIS_TARGET MATCHES "^cleanall")
               # For protection
               if(EXISTS ${ARGV2})
                   add_custom_target(${THIS_TARGET} 
                       COMMAND rm -rf "${ARGV2}/*" 
                       COMMENT "Cleaning all files under ${ARGV2}")
               endif(EXISTS ${ARGV2})
            elseif(THIS_TARGET MATCHES "^ls")
                add_custom_target(${THIS_TARGET}
                    COMMAND ls -d "${ARGV2}")
            elseif(THIS_TARGET MATCHES "^cmake")
                add_custom_target(${THIS_TARGET}
                    COMMAND ${CMAKE_COMMAND} ..
                    WORKING_DIRECTORY ${ARGV2}
                    COMMENT "Running cmake for ${ARGV2}")
            else()
                add_custom_target(${THIS_TARGET}
                    COMMAND ${CMAKE_MAKE_PROGRAM} ${TARGET}
                    WORKING_DIRECTORY ${ARGV2}
                    COMMENT "Running make ${TARGET} for ${ARGV2}")
            endif()
        else(${BUILD_COMMAND})
            if(THIS_TARGET STREQUAL "makeall")
                add_custom_target(${THIS_TARGET} ALL)
            elseif(THIS_TARGET STREQUAL "clean")
                # The clean target gets auto-generated, so we must build our own version to
                # traverse the other Makefiles
                add_custom_target(cleaner
                    COMMAND ${CMAKE_MAKE_PROGRAM} clean
                    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
            else()
                add_custom_target(${THIS_TARGET})
            endif()
        endif(${BUILD_COMMAND})
        
        # All the make-like targets depends on cmake
        if(NOT ${THIS_TARGET} MATCHES "^clean" AND 
           NOT ${THIS_TARGET} MATCHES "^cmake" AND 
           NOT ${THIS_TARGET} MATCHES "^ls")
            add_dependencies(${THIS_TARGET} cmake${TARGET_SUFFIX})
        endif()
    endforeach(TARGET ${TARGETS})
endmacro(build_targets)

macro(add_target_dependencies TARGET_SUFFIX DEPEND_TARGET_SUFFIX)
    foreach(TARGET ${TARGETS})
        if(${TARGET}${TARGET_SUFFIX} STREQUAL "clean")
            add_dependencies(cleaner ${TARGET}${DEPEND_TARGET_SUFFIX})
        else()
            add_dependencies(${TARGET}${TARGET_SUFFIX} ${TARGET}${DEPEND_TARGET_SUFFIX})
        endif()
    endforeach(TARGET ${TARGETS})
endmacro()

macro(create_targets)
    # Build the top-level targets
    build_targets("" NO)
    
    foreach(THIS_CONFIG ${CONFIG_DIRS})
        if (NOT TARGET ${THIS_CONFIG})
            # Top-level target for this configuration
            add_custom_target(${THIS_CONFIG})
            build_targets(_${THIS_CONFIG} NO)
            add_dependencies(${THIS_CONFIG} makeall_${THIS_CONFIG})
        endif (NOT TARGET ${THIS_CONFIG})

        foreach(THIS_CONFIG_GLOB ${ARGN})
            file(GLOB THESE_CONFIG_DIRS "${THIS_CONFIG_GLOB}/${THIS_CONFIG}")
 
            foreach(THIS_CONFIG_DIR IN ITEMS ${THESE_CONFIG_DIRS})
        	    get_filename_component(THIS_CMAKE_BINARY_DIR ${THIS_CONFIG_DIR} ABSOLUTE)
        	    get_filename_component(THIS_CMAKE_SOURCE_DIR ${THIS_CMAKE_BINARY_DIR} DIRECTORY)
        	    get_filename_component(THIS_CMAKE_TARGET_NAME ${THIS_CMAKE_SOURCE_DIR} NAME)
        	    
                # This should be the case, but just to be sure
                if(EXISTS ${THIS_CMAKE_BINARY_DIR})
                    message(STATUS "Configuring targets for ${THIS_CMAKE_BINARY_DIR}...")
        
                    # Top-level targets for this component
                    build_targets(_${THIS_CMAKE_TARGET_NAME} NO)
                    if(NOT TARGET ${THIS_CMAKE_TARGET_NAME})
                        add_custom_target(${THIS_CMAKE_TARGET_NAME})
                        add_dependencies(${THIS_CMAKE_TARGET_NAME} makeall_${THIS_CMAKE_TARGET_NAME})
                    endif()
                    
                    build_targets(_${THIS_CMAKE_TARGET_NAME}_${THIS_CONFIG} YES ${THIS_CMAKE_BINARY_DIR})
                    if(NOT TARGET ${THIS_CMAKE_TARGET_NAME}_${THIS_CONFIG})
                        add_custom_target(${THIS_CMAKE_TARGET_NAME}_${THIS_CONFIG})
                        add_dependencies(${THIS_CMAKE_TARGET_NAME}_${THIS_CONFIG} makeall_${THIS_CMAKE_TARGET_NAME}_${THIS_CONFIG})
                    endif()
                    
                    add_target_dependencies("" _${THIS_CMAKE_TARGET_NAME})
                    add_target_dependencies(_${THIS_CONFIG} _${THIS_CMAKE_TARGET_NAME}_${THIS_CONFIG})
        
                    file(STRINGS ${THIS_CMAKE_SOURCE_DIR}/CMakeLists.txt IS_API_DEPENDENT
                         REGEX "find_package *\\(.*TmxApi")
                    file(STRINGS ${THIS_CMAKE_SOURCE_DIR}/CMakeLists.txt IS_UTILS_DEPENDENT
                         REGEX "include *\\( *TMX_Plugin")
                            
                    if(${THIS_CMAKE_TARGET_NAME} STREQUAL "TmxApi")
                        # TMX API depends on the ASN.1 J2735 library
                        add_target_dependencies(_${THIS_CMAKE_TARGET_NAME} _Asn_J2735)
                        add_target_dependencies(_${THIS_CMAKE_TARGET_NAME}_${THIS_CONFIG} _Asn_J2735_${THIS_CONFIG})           
                    elseif(IS_API_DEPENDENT OR ${THIS_CMAKE_TARGET_NAME} MATCHES "Arada.*Plugin")
                        # These depend on TMX API only
                        add_target_dependencies(_${THIS_CMAKE_TARGET_NAME} _TmxApi)
                        add_target_dependencies(_${THIS_CMAKE_TARGET_NAME}_${THIS_CONFIG} _TmxApi_${THIS_CONFIG})
                    elseif(IS_UTILS_DEPENDENT)
                        # Every plugin depends on TMX utils
                        add_target_dependencies(_${THIS_CMAKE_TARGET_NAME} _TmxUtils )
                        add_target_dependencies(_${THIS_CMAKE_TARGET_NAME}_${THIS_CONFIG} _TmxUtils_${THIS_CONFIG} )
                    endif()
                                
                    add_target_dependencies(_${THIS_CMAKE_TARGET_NAME} _${THIS_CMAKE_TARGET_NAME}_${THIS_CONFIG})
                endif(EXISTS ${THIS_CMAKE_BINARY_DIR})
            endforeach(THIS_CONFIG_DIR ${THESE_CONFIG_DIRS})
        endforeach(THIS_CONFIG_GLOB ${ARGN})    
    endforeach(THIS_CONFIG ${CONFIG_DIRS})
    
    # Remove all the cached argument variables so next run is fresh
    unset(CONFIG_DIRS CACHE)
    unset(CMAKE_CLEAN CACHE)
    unset(MAKE_MAKE CACHE)
    unset(MAKE CACHE)
    unset(MAKE_TARGET CACHE)
endmacro()
