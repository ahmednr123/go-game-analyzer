cmake_minimum_required(VERSION 3.16)
project(go-game LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CONFIG_TYPE "linux_raw" CACHE STRING "Which config file to use")
message(STATUS "Using config for: ${CONFIG_TYPE}")

set(BUILD_TYPE "linux" CACHE STRING "Which build type")
message(STATUS "Building for: ${BUILD_TYPE}")

# -----------------------------------------
# Source files
# -----------------------------------------
set(SOURCES
    src/main.cpp
    src/draw.cpp
    src/board.cpp
    src/state.cpp
    src/rules.cpp
    src/katago.cpp
    src/katago_engine.cpp
    src/config.cpp
    src/sound.cpp
    src/error.cpp
    src/theme.cpp
)

set(HEADERS
    src/base.hpp
    src/actions.hpp
    src/board.hpp
    src/draw.hpp
    src/error.hpp
    src/helpers.hpp
    src/state.hpp
    src/utils.hpp
    src/rules.hpp
    src/test.hpp
    src/compute.hpp
    src/katago.hpp
    src/katago_engine.hpp
    src/katago_settings.hpp
    src/config.hpp
    src/sound.hpp
    src/theme.hpp
)

add_executable(go-game ${SOURCES} ${HEADERS})

set(CMAKE_PREFIX_PATH ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_IGNORE_PATH "/usr/local/include" "/usr/include" "/usr/lib" "/usr/local/lib")
set(PKG_CONFIG_EXECUTABLE "")

# -----------------------------------------
# Windows (MinGW cross-compile) build
# -----------------------------------------
if (MINGW)
    message(STATUS "Building with MinGW")

    add_compile_definitions(WINDOWS)

    target_include_directories(go-game PRIVATE
        ${CMAKE_SOURCE_DIR}/lib/include
    )

    target_link_directories(go-game PRIVATE
        ${CMAKE_SOURCE_DIR}/lib/windows
    )

    target_link_libraries(go-game PRIVATE
        SDL3
        SDL3_ttf
        SDL3_mixer
        freetype
    )

    # Copy DLLs
    add_custom_command(
        TARGET go-game POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_SOURCE_DIR}/lib/windows/SDL3.dll
            ${CMAKE_SOURCE_DIR}/lib/windows/SDL3_ttf.dll
            ${CMAKE_SOURCE_DIR}/lib/windows/SDL3_mixer.dll
            ${CMAKE_SOURCE_DIR}/lib/windows/freetype.dll
            $<TARGET_FILE_DIR:go-game>
    )

    # Copy assets
    add_custom_command(
        TARGET go-game POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/assets
            $<TARGET_FILE_DIR:go-game>/assets
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_SOURCE_DIR}/config.${CONFIG_TYPE}.json
            $<TARGET_FILE_DIR:go-game>/config.json
    )

    return()
endif()


# -----------------------------------------
# Native Linux
# -----------------------------------------

message(STATUS "Using manually provided SDL3 libraries")

# Include directory for custom SDL build
target_include_directories(go-game PRIVATE
    ${CMAKE_SOURCE_DIR}/lib/include
)

# Library search path
target_link_directories(go-game PRIVATE
    ${CMAKE_SOURCE_DIR}/lib/linux
)

# Link your custom .so files (no find_package)
target_link_libraries(go-game PRIVATE
    SDL3
    SDL3_ttf
    SDL3_mixer
    freetype
)

if(BUILD_TYPE STREQUAL "linux")
# Copy library files
    add_custom_command(
        TARGET go-game POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_SOURCE_DIR}/lib/linux/libSDL3.so.0.3.3
            $<TARGET_FILE_DIR:go-game>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_SOURCE_DIR}/lib/linux/libSDL3_ttf.so.0.3.0
            $<TARGET_FILE_DIR:go-game>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_SOURCE_DIR}/lib/linux/libSDL3_mixer.so.0.1.0
            $<TARGET_FILE_DIR:go-game>
    )

    execute_process(COMMAND bash "${CMAKE_SOURCE_DIR}/scripts/generate-symlinks.sh"
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
endif()

# Copy assets
add_custom_command(
    TARGET go-game POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:go-game>/assets
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/config.${CONFIG_TYPE}.json
        $<TARGET_FILE_DIR:go-game>/config.json
)
