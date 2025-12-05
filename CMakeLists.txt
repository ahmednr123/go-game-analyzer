cmake_minimum_required(VERSION 3.16)
project(MCE LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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

add_executable(MCE ${SOURCES} ${HEADERS})

set(CMAKE_PREFIX_PATH ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_IGNORE_PATH "/usr/local/include" "/usr/include" "/usr/lib" "/usr/local/lib")
set(PKG_CONFIG_EXECUTABLE "")

# -----------------------------------------
# Windows (MinGW cross-compile) build
# -----------------------------------------
if (MINGW)
    message(STATUS "Building with MinGW")

    add_compile_definitions(WINDOWS)

    target_include_directories(MCE PRIVATE
        ${CMAKE_SOURCE_DIR}/lib/include
    )

    target_link_directories(MCE PRIVATE
        ${CMAKE_SOURCE_DIR}/lib/windows
    )

    target_link_libraries(MCE PRIVATE
        SDL3
        SDL3_ttf
        SDL3_mixer
        freetype
    )

    # Copy DLLs
    add_custom_command(
        TARGET MCE POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_SOURCE_DIR}/lib/windows/SDL3.dll
            ${CMAKE_SOURCE_DIR}/lib/windows/SDL3_ttf.dll
            ${CMAKE_SOURCE_DIR}/lib/windows/SDL3_mixer.dll
            ${CMAKE_SOURCE_DIR}/lib/windows/freetype.dll
            $<TARGET_FILE_DIR:MCE>
    )

    # Copy assets
    add_custom_command(
        TARGET MCE POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory
            $<TARGET_FILE_DIR:MCE>/assets/fonts
        COMMAND ${CMAKE_COMMAND} -E make_directory
            $<TARGET_FILE_DIR:MCE>/assets/themes
        COMMAND ${CMAKE_COMMAND} -E make_directory
            $<TARGET_FILE_DIR:MCE>/assets/sound
        COMMAND ${CMAKE_COMMAND} -E make_directory
            $<TARGET_FILE_DIR:MCE>/assets/music
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_SOURCE_DIR}/assets/fonts/Lexend.ttf
            $<TARGET_FILE_DIR:MCE>/assets/fonts
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_SOURCE_DIR}/assets/themes/*
            $<TARGET_FILE_DIR:MCE>/assets/themes
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_SOURCE_DIR}/assets/sound/*
            $<TARGET_FILE_DIR:MCE>/assets/sound
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_SOURCE_DIR}/assets/music/*
            $<TARGET_FILE_DIR:MCE>/assets/music
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_SOURCE_DIR}/config.windows.json
            $<TARGET_FILE_DIR:MCE>/config.json
    )

    return()
endif()


# -----------------------------------------
# Native Linux
# -----------------------------------------

message(STATUS "Using manually provided SDL3 libraries")

# Include directory for custom SDL build
target_include_directories(MCE PRIVATE
    ${CMAKE_SOURCE_DIR}/lib/include
)

# Library search path
target_link_directories(MCE PRIVATE
    ${CMAKE_SOURCE_DIR}/lib/linux
)

# Link your custom .so files (no find_package)
target_link_libraries(MCE PRIVATE
    SDL3
    SDL3_ttf
    SDL3_mixer
    freetype
)

# Copy library files
add_custom_command(
    TARGET MCE POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/lib/linux/*
        $<TARGET_FILE_DIR:MCE>
)

# Copy assets
add_custom_command(
    TARGET MCE POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
        $<TARGET_FILE_DIR:MCE>/assets/fonts
    COMMAND ${CMAKE_COMMAND} -E make_directory
        $<TARGET_FILE_DIR:MCE>/assets/themes
    COMMAND ${CMAKE_COMMAND} -E make_directory
        $<TARGET_FILE_DIR:MCE>/assets/sound
    COMMAND ${CMAKE_COMMAND} -E make_directory
        $<TARGET_FILE_DIR:MCE>/assets/music
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/assets/fonts/Lexend.ttf
        $<TARGET_FILE_DIR:MCE>/assets/fonts
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/assets/themes/*
        $<TARGET_FILE_DIR:MCE>/assets/themes
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/assets/sound/*
        $<TARGET_FILE_DIR:MCE>/assets/sound
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/assets/music/*
        $<TARGET_FILE_DIR:MCE>/assets/music
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/config.macos-linux.json
        $<TARGET_FILE_DIR:MCE>/config.json
)

