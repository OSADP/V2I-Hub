set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" 
    ${CMAKE_SOURCE_DIR}/../../../../TMX-OAM/Build/Modules
    ${CMAKE_SOURCE_DIR}/../../../TMX-OAM/Build/Modules
    $ENV{HOME}/TMX-OAM/Build/Modules
    $ENV{TMX_OAM_HOME}/Build/Modules
)
cmake_minimum_required(VERSION 2.8.12)

project(ExtractedMessageTypes NONE)

include(TMX_C_Project)

find_package(TmxCommon COMPONENTS Asn_J2735 TmxMessages TmxApi)

set(GEN_SRC_DIR ${CMAKE_BINARY_DIR}/generated-src)
file(MAKE_DIRECTORY ${GEN_SRC_DIR})

set(TMX_MESSAGE_PATH ${TMX_MESSAGE_DIRS} ${TmxApi_INCLUDE_DIR}/tmx/j2735_messages ${TmxMessages_INCLUDE_DIR})

foreach(DIR ${TMX_MESSAGE_PATH})
    if(EXISTS ${DIR})
        file(GLOB_RECURSE INCS "${DIR}/*.h*")
 
        foreach(INC ${INCS})
            get_filename_component(INC_DIR ${INC} DIRECTORY)
            include_directories(${INC_DIR})
 
            get_filename_component(IncludeFile ${INC} NAME)
            unset(ClassName)
            
            file(READ ${INC} inc_text)
            string(REGEX MATCH "class  *([a-zA-Z0-9_]*) *: *public  *tmx::message" inc_match ${inc_text})
            if(inc_match)
                set(ClassName ${CMAKE_MATCH_1})
            else(inc_match)
                string(REGEX MATCH "typedef  *TmxJ2735Message<[a-zA-Z0-9_]*>  *([a-zA-Z0-9_]*)" inc2_match ${inc_text})
                if(inc2_match)
                     set(ClassName ${CMAKE_MATCH_1})
                endif(inc2_match)
            endif(inc_match)
            
            if(ClassName)
                configure_file(${CMAKE_SOURCE_DIR}/src/ExtractMessageTypeTemplate.cpp
                               ${GEN_SRC_DIR}/ExtractMessageType${ClassName}.cpp)
            endif()
        endforeach(INC ${INCS})     
    endif(EXISTS ${DIR})
endforeach(DIR ${TMX_MESSAGE_DIRS}) 

file(GLOB GENERATED_SRC "${GEN_SRC_DIR}/*.cpp")
add_executable(ExtractMessageTypes ${CMAKE_SOURCE_DIR}/src/Main.cpp ${GENERATED_SRC})
target_include_directories(ExtractMessageTypes PUBLIC ${CMAKE_SOURCE_DIR}/src)

add_custom_command(TARGET ExtractMessageTypes POST_BUILD 
    COMMAND ${CMAKE_BINARY_DIR}/ExtractMessageTypes WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

