set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" 
    ${CMAKE_SOURCE_DIR}/../../../../TMX-OAM/Build/Modules
    ${CMAKE_SOURCE_DIR}/../../../TMX-OAM/Build/Modules
    $ENV{HOME}/TMX-OAM/Build/Modules
    $ENV{TMX_OAM_HOME}/Build/Modules
)
cmake_minimum_required(VERSION 2.8.12)

project( tmxutils NONE )

set(CPACK_PACKAGE_NAME "lib${PROJECT_NAME}")
include(TMX_C_Project)
include(CPack)

file(GLOB_RECURSE HEADERS "src/*.h*")
file(GLOB_RECURSE SOURCES "src/[A-Z]*.c*")

find_package(TmxCommon COMPONENTS Asn_J2735 TmxApi TmxMessages REQUIRED)

add_library (${PROJECT_NAME}Static STATIC ${SOURCES})
set_target_properties(${PROJECT_NAME}Static PROPERTIES OUTPUT_NAME ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME}Static ${TmxApi_LIBRARIES} ${Asn_J2735_LIBRARY} rt)

install (TARGETS ${PROJECT_NAME}Static DESTINATION lib)
install (FILES ${HEADERS} DESTINATION include)

# Add support to the test executable so it can find the includes and lib of the base library being tested.
target_include_directories(${CTEST_TEST_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/src )
target_link_libraries(${CTEST_TEST_NAME} ${PROJECT_NAME}Static)
