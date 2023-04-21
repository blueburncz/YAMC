cmake_minimum_required(VERSION 3.23)

project(yamc)

set(CMAKE_CXX_STANDARD 17)

set(SOURCES
    src/main.cpp
    )

find_library(LIBASSIMP
    NAMES assimp-vc143-mt assimp.5
    PATHS lib/
    )

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE include/)

target_link_libraries(${PROJECT_NAME} ${LIBASSIMP})

## Copy files to the build folder
# Assimp
if(WIN32)
    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_CURRENT_SOURCE_DIR}/bin/assimp-vc143-mt.dll
            $<TARGET_PROPERTY:${PROJECT_NAME},BINARY_DIR>
            )
elseif(APPLE)
    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_CURRENT_SOURCE_DIR}/lib/libassimp.5.dylib
            $<TARGET_PROPERTY:${PROJECT_NAME},BINARY_DIR>
            )
endif()

# Assimp license
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/bin/LICENSE-Assimp
        $<TARGET_PROPERTY:${PROJECT_NAME},BINARY_DIR>
        )
