cmake_minimum_required(VERSION 2.8.11)
project(exit2destination LANGUAGES C CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -std=c++11")

find_package(Threads REQUIRED)

# Vendored Libosmium
if (ENABLE_MASON)
    set(MASON_COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/.mason/mason)
    include(${CMAKE_CURRENT_SOURCE_DIR}/.mason/mason.cmake)

    mason_use(libosmium VERSION 2.10.3 HEADER_ONLY)
    include_directories(${MASON_PACKAGE_libosmium_INCLUDE_DIRS})
    set(OSMIUM_INCLUDE_DIRS ${MASON_PACKAGE_libosmium_INCLUDE_DIRS})

    mason_use(protozero VERSION 1.4.2 HEADER_ONLY)
    include_directories(${MASON_PACKAGE_protozero_INCLUDE_DIRS})
    set(PROTOZERO_INCLUDE_DIRS ${MASON_PACKAGE_protozero_INCLUDE_DIRS})

    # use system zlib, mason zlib does not work with libosmium
    find_package(ZLIB REQUIRED)
    include_directories(${ZLIB_INCLUDE_DIRS})

    add_executable(${PROJECT_NAME} ${PROJECT_NAME}.cc)
    target_link_libraries(${PROJECT_NAME} ${OSMIUM_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${ZLIB_LIBRARY})
    target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE ${OSMIUM_INCLUDE_DIRS})
else()
    # expects a vendored libosmium at /third_party
    # explicit inclusion of protozero and zlib dependencies are not required
    # if libosmium is vendored. The find_package(Osmium) call calls libosmium's
    # cmake files that find required dependencies in the system
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/third_party/libosmium/cmake")
    set(OSMIUM_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/libosmium/include")
    find_package(Osmium REQUIRED COMPONENTS pbf)

    add_executable(${PROJECT_NAME} ${PROJECT_NAME}.cc)
    target_link_libraries(${PROJECT_NAME} ${OSMIUM_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
    target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE ${OSMIUM_INCLUDE_DIRS})
endif()

