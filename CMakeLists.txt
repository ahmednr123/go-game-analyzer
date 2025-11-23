cmake_minimum_required(VERSION 3.16)
project(MCE LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# --- Add all C++ source files here ---
set(SOURCES
    src/main.cpp
    src/draw.cpp
    src/board.cpp
)

set(HEADERS
    src/base.hpp
    src/actions.hpp
    src/board.hpp
    src/draw.hpp
    src/error.hpp
    src/helpers.hpp
    src/undo.hpp
    src/utils.hpp
)

add_executable(MCE ${SOURCES} ${HEADERS})

# --- Try pkg-config first ---
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
    # --- Fallback: CMake config package ---
    find_package(SDL3 CONFIG QUIET)
    find_package(SDL3_ttf CONFIG QUIET)

    if (TARGET SDL3::SDL3 AND TARGET SDL3_ttf::SDL3_ttf)
        message(STATUS "Using SDL3 + SDL3_ttf (CMake package)")
        target_link_libraries(MCE PRIVATE
            SDL3::SDL3
            SDL3_ttf::SDL3_ttf
        )
    else()
        message(FATAL_ERROR
            "SDL3_ttf not found! Install SDL3_ttf or set CMAKE_PREFIX_PATH.")
    endif()
endif()

# Copy font assets to the build output
add_custom_command(
    TARGET MCE POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
        $<TARGET_FILE_DIR:MCE>/assets/fonts
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/assets/fonts/Lexend.ttf
        $<TARGET_FILE_DIR:MCE>/assets/fonts
)

