cmake_minimum_required(VERSION 2.8.11)
project(exit2destination LANGUAGES C CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -std=c++11")

find_package(Boost REQUIRED)
find_package(Threads REQUIRED)

# Vendored Libosmium
if (ENABLE_MASON)
    set(MASON_COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/.mason/mason)
    include(${CMAKE_CURRENT_SOURCE_DIR}/.mason/mason.cmake)
    mason_use(protozero VERSION 1.4.2)
    mason_use(zlib VERSION 1.2.8)
else()
    # expects a vendored libosmium at /third_party
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/third_party/libosmium/cmake")
    set(OSMIUM_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/libosmium/include")
    find_package(Osmium)
endif()

add_executable(${PROJECT_NAME} ${PROJECT_NAME}.cc)
target_link_libraries(${PROJECT_NAME} ${OSMIUM_LIBRARIES} ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE ${OSMIUM_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})
