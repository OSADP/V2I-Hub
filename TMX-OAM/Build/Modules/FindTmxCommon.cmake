# - Try to find the common component include directories and libraries
#   which may be under the TMX_CORE area or under the specific Common
#   area for the current source directories
#
# Once done, this will define the following variables:
#   <component>_FOUND         - System has or does not have the common component
#   <component>_INCLUDE_DIR   - The common component include directories
#   <component>_LIBRARIES     - The common component library full path
#   <component>_LIBRARY_DIR   - The common component library directory
#

if(MIPS_BUILD)
    set(LIB_SUFFIXES "MIPS-${CMAKE_BUILD_TYPE}")
    set(NO_DEFAULTS "NO_DEFAULT_PATH")
elseif(ARM_BUILD)
    set(LIB_SUFFIXES "ARM-${CMAKE_BUILD_TYPE}")
    set(NO_DEFAULTS "NO_DEFAULT_PATH")
else(MIPS_BUILD)
    set(LIB_SUFFIXES "${CMAKE_BUILD_TYPE}")
endif(MIPS_BUILD)

foreach(COMPONENT ${TmxCommon_FIND_COMPONENTS})
    string(TOLOWER ${COMPONENT} COMPONENT_LC)
    
    # Handle the unusual cases 
    if (${COMPONENT} STREQUAL "Asn_J2735")
        set(HEADER_NAME asn_j2735_r41/BasicSafetyMessage.h)
        set(COMPONENT_LC "${COMPONENT_LC}_r41")
    elseif(${COMPONENT} STREQUAL "TmxApi")
        set(HEADER_NAME tmx/tmx.h)
    else()
        set(HEADER_NAME version.h)
    endif()

    find_path(${COMPONENT}_INCLUDE_DIR ${HEADER_NAME}
              HINTS ${TMX_CORE_DIR} ${TMX_CORE_DIR}/${COMPONENT}
                    ${CMAKE_SOURCE_DIR}/../Common
                    ${CMAKE_SOURCE_DIR}/../../Common
                    ${TMX_CORE_DIR}/../Common
              PATH_SUFFIXES ${COMPONENT}/include ${COMPONENT}/src )
    
    unset(HEADER_NAME)

    if (NOT ${COMPONENT}_INCLUDE_DIR AND 
            (TmxCommon_FIND_REQUIRED OR TmxCommon_FIND_REQUIRED_${COMPONENT}))
        message(FATAL_ERROR "Could not find common component ${COMPONENT}")
    endif()
    
    set(${COMPONENT}_INCLUDE_DIRS ${${COMPONENT}_INCLUDE_DIR})
    include_directories( ${${COMPONENT}_INCLUDE_DIR} )
    mark_as_advanced( ${COMPONENT}_INCLUDE_DIR )
    
    if (EXISTS ${${COMPONENT}_INCLUDE_DIR}/CMakeLists.txt)
        get_filename_component(LIB_BASE_DIR ${${COMPONENT}_INCLUDE_DIR} ABSOLUTE)
    else()
        get_filename_component(LIB_BASE_DIR ${${COMPONENT}_INCLUDE_DIR} DIRECTORY)
    endif()

    if (NOT ${${COMPONENT}_INCLUDE_DIR} MATCHES ".*/include" OR EXISTS ${LIB_BASE_DIR}/CMakeLists.txt)
        find_library(${COMPONENT}_LIBRARY NAMES ${COMPONENT} ${COMPONENT_LC} 
                HINTS ${TMX_CORE_DIR} ${TMX_CORE_DIR}/${COMPONENT}
                      ${CMAKE_SOURCE_DIR}/../Common
                      ${CMAKE_SOURCE_DIR}/../../Common
                PATH_SUFFIXES ${COMPONENT}/${LIB_SUFFIXES} ${NO_DEFAULTS})     
       
        # If library is not found, it may be built later, so at least add link directory
        if(NOT ${COMPONENT}_LIBRARY)
            set(${COMPONENT}_LIBRARY_DIR ${LIB_BASE_DIR}/${LIB_SUFFIXES})
            file(STRINGS ${LIB_BASE_DIR}/CMakeLists.txt IS_STATIC_LIB REGEX "add_library.*STATIC")
            if (IS_STATIC_LIB)
                set(${COMPONENT}_LIBRARY "${${COMPONENT}_LIBRARY_DIR}/lib${COMPONENT_LC}.a")
            else()
                set(${COMPONENT}_LIBRARY -l${COMPONENT_LC})
            endif()
        else(NOT ${COMPONENT}_LIBRARY)
            include(FindPackageHandleStandardArgs)
            find_package_handle_standard_args(${COMPONENT} DEFAULT_MSG ${COMPONENT}_INCLUDE_DIR ${COMPONENT}_LIBRARY)
        
            get_filename_component(${COMPONENT}_LIBRARY_DIR ${${COMPONENT}_LIBRARY} DIRECTORY)
        endif(NOT ${COMPONENT}_LIBRARY)
            
        set(${COMPONENT}_LIBRARIES ${${COMPONENT}_LIBRARY} )
        link_directories( ${${COMPONENT}_LIBRARY_DIR} )

        mark_as_advanced( ${COMPONENT}_LIBRARY ${COMPONENT}_INCLUDE_DIR )
    endif()
endforeach()
