set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" 
    ${CMAKE_SOURCE_DIR}/../../../../TMX-OAM/Build/Modules
    ${CMAKE_SOURCE_DIR}/../../../TMX-OAM/Build/Modules
    $ENV{HOME}/TMX-OAM/Build/Modules
    $ENV{TMX_OAM_HOME}/Build/Modules
)
cmake_minimum_required(VERSION 2.8.12)

project( AradaReceiverPlugin NONE )

set(MIPS_BUILD ON)

set( CMAKE_C_DIALECT "-std=gnu89" )
set( CMAKE_CXX_DIALECT " " )

include(TMX_Plugin_Project)
