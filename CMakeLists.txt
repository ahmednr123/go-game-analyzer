cmake_minimum_required(VERSION 3.16)
project(MCE LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# -----------------------------------------
# Source files
# -----------------------------------------
set(SOURCES
    src/main.cpp
    src/draw.cpp
    src/board.cpp
    src/state.cpp
    src/rules.cpp
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
)

add_executable(MCE ${SOURCES} ${HEADERS})


# -----------------------------------------
# Windows (MinGW cross-compile) build
# -----------------------------------------
if (MINGW)
    message(STATUS "Building with MinGW")

    target_include_directories(MCE PRIVATE
        ${CMAKE_SOURCE_DIR}/lib/include
    )

    target_link_directories(MCE PRIVATE
        ${CMAKE_SOURCE_DIR}/lib
    )

    target_link_libraries(MCE PRIVATE
        SDL3
        SDL3_ttf
        freetype
    )

    # Copy DLLs
    add_custom_command(
        TARGET MCE POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_SOURCE_DIR}/lib/SDL3.dll
            ${CMAKE_SOURCE_DIR}/lib/SDL3_ttf.dll
            ${CMAKE_SOURCE_DIR}/lib/freetype.dll
            $<TARGET_FILE_DIR:MCE>
    )

    # Copy assets
    add_custom_command(
        TARGET MCE POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory
            $<TARGET_FILE_DIR:MCE>/assets/fonts
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_SOURCE_DIR}/assets/fonts/Lexend.ttf
            $<TARGET_FILE_DIR:MCE>/assets/fonts
    )

    return()
endif()


# -----------------------------------------
# Native Linux / macOS
# Use official SDL3 CMake packages only
# -----------------------------------------
find_package(SDL3 REQUIRED CONFIG)
find_package(SDL3_ttf REQUIRED CONFIG)

message(STATUS "Using SDL3 CMake packages → OK")

target_link_libraries(MCE PRIVATE
    SDL3::SDL3
    SDL3_ttf::SDL3_ttf
)


# -----------------------------------------
# Copy assets on Linux/macOS
# -----------------------------------------
add_custom_command(
    TARGET MCE POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
        $<TARGET_FILE_DIR:MCE>/assets/fonts
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/assets/fonts/Lexend.ttf
        $<TARGET_FILE_DIR:MCE>/assets/fonts
)

