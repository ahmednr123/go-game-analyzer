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
# Detect MinGW cross compilation
# -----------------------------------------
if (MINGW)
    message(STATUS "Cross compiling with MinGW → manual SDL3 setup")

    # Your Windows SDL3 libs are in lib/
    target_include_directories(MCE PRIVATE
        ${CMAKE_SOURCE_DIR}/lib/include
    )

    target_link_directories(MCE PRIVATE
        ${CMAKE_SOURCE_DIR}/lib
    )

    # Link Windows import libraries
    target_link_libraries(MCE PRIVATE
        SDL3
        SDL3_ttf
        freetype
    )

    # Copy DLLs next to the exe
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
# Native Linux / macOS build
# -----------------------------------------
find_package(PkgConfig QUIET)
if (PkgConfig_FOUND)
    pkg_check_modules(SDL3_PKG QUIET sdl3)
    pkg_check_modules(SDL3_TTF_PKG QUIET sdl3-ttf)
endif()

if (SDL3_PKG_FOUND AND SDL3_TTF_PKG_FOUND)
    message(STATUS "Using SDL3 + SDL3_ttf (pkg-config)")

    target_include_directories(MCE PRIVATE
        ${SDL3_PKG_INCLUDE_DIRS}
        ${SDL3_TTF_PKG_INCLUDE_DIRS}
    )

    target_link_libraries(MCE PRIVATE
        ${SDL3_PKG_LIBRARIES}
        ${SDL3_TTF_PKG_LIBRARIES}
    )

    target_compile_options(MCE PRIVATE
        ${SDL3_PKG_CFLAGS_OTHER}
        ${SDL3_TTF_PKG_CFLAGS_OTHER}
    )

else()
    # --- Fallback: CMake config ---
    find_package(SDL3 CONFIG QUIET)
    find_package(SDL3_ttf CONFIG QUIET)

    if (TARGET SDL3::SDL3 AND TARGET SDL3_ttf::SDL3_ttf)
        message(STATUS "Using SDL3 + SDL3_ttf (CMake package)")
        target_link_libraries(MCE PRIVATE SDL3::SDL3 SDL3_ttf::SDL3_ttf)
    else()
        message(FATAL_ERROR "SDL3_ttf not found! Install SDL3_ttf or set CMAKE_PREFIX_PATH.")
    endif()
endif()

# Copy assets (Linux/macOS)
add_custom_command(
    TARGET MCE POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
        $<TARGET_FILE_DIR:MCE>/assets/fonts
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/assets/fonts/Lexend.ttf
        $<TARGET_FILE_DIR:MCE>/assets/fonts
)
