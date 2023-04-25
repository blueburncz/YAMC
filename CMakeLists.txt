cmake_minimum_required(VERSION 3.23)

project(yamc)

set(CMAKE_CXX_STANDARD 17)

set(SOURCES
    src/Args.cpp
    src/Config.cpp
    src/main.cpp
    src/writing.cpp
    )

set(OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dist)

find_library(LIBASSIMP
    NAMES assimp-vc143-mt assimp.5
    PATHS lib/
    )

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE include/)

target_link_libraries(${PROJECT_NAME} ${LIBASSIMP})

## Export files to dist folder
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${OUTPUT_DIR}
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${OUTPUT_DIR}
    )

# Assimp
if(WIN32)
    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_CURRENT_SOURCE_DIR}/bin/assimp-vc143-mt.dll
            ${OUTPUT_DIR}
            )
elseif(APPLE)
    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_CURRENT_SOURCE_DIR}/lib/libassimp.5.dylib
            ${OUTPUT_DIR}
            )
endif()

# Assimp license
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/bin/LICENSE-Assimp
        ${OUTPUT_DIR}
        )
