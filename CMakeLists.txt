cmake_minimum_required(VERSION 3.20)

project(Walrus
    VERSION 0.1.0
    DESCRIPTION "A modern C GUI toolkit for Linux using Wayland and X11"
    LANGUAGES C
)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ============================================================
# Options
# ============================================================

option(WALRUS_BUILD_EXAMPLES "Build examples" ON)
option(WALRUS_ENABLE_WAYLAND "Enable Wayland backend" ON)
option(WALRUS_ENABLE_X11 "Enable X11 backend" OFF)

# ============================================================
# Dependencies
# ============================================================

find_package(PkgConfig REQUIRED)

# EGL
pkg_check_modules(EGL REQUIRED egl)

# ============================================================
# Library sources
# ============================================================

set(WALRUS_SOURCES
  src/walrus.c

  # Backend sources
  src/backend/backend.c

  # Core sources
  src/core/window.c

  # Renderer sources
  src/renderer/renderer.c
  src/renderer/opengl/egl.c
  src/renderer/opengl/opengl.c

  # External sources
  external/glad/src/egl.c
  external/glad/src/glad.c
)

# ============================================================
# Wayland
# ============================================================

if(WALRUS_ENABLE_WAYLAND)

    pkg_check_modules(WAYLAND wayland-client)
    pkg_check_modules(WAYLAND_EGL wayland-egl)

    if(WAYLAND_FOUND AND WAYLAND_EGL_FOUND)

        message(STATUS "Wayland backend enabled")

        list(APPEND WALRUS_SOURCES
            src/backend/wayland/wayland.c
            src/backend/wayland/xdg-shell-protocol.c
        )

    elseif(WAYLAND_FOUND)

        message(WARNING "Wayland found but wayland-egl not found; backend disabled")

    else()

        message(WARNING "Wayland not found")

    endif()

    if(WAYLAND_EGL_FOUND)
        list(APPEND WALRUS_INCLUDE_DIRS ${WAYLAND_EGL_INCLUDE_DIRS})
        list(APPEND WALRUS_LIBRARY_DIRS ${WAYLAND_EGL_LIBRARY_DIRS})
        list(APPEND WALRUS_LIBRARIES ${WAYLAND_EGL_LIBRARIES})
    endif()

endif()

# ============================================================
# X11
# ============================================================

if(WALRUS_ENABLE_X11)

    pkg_check_modules(X11 x11)

    if(X11_FOUND)

        message(STATUS "X11 backend enabled")

        list(APPEND WALRUS_SOURCES
            src/backend/x11/x11.c
        )

    else()

        message(WARNING "X11 not found")

    endif()

endif()

# ============================================================
# Main library
# ============================================================

add_library(walrus SHARED
    ${WALRUS_SOURCES}
)

# ============================================================
# Include directories
# ============================================================

target_include_directories(walrus
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include

    PRIVATE
        ${PROJECT_SOURCE_DIR}/external/glad/include
        ${EGL_INCLUDE_DIRS}
        ${WALRUS_INCLUDE_DIRS}
)

# ============================================================
# EGL and extra library directories
# ============================================================

target_link_directories(walrus PRIVATE
    ${EGL_LIBRARY_DIRS}
    ${WALRUS_LIBRARY_DIRS}
)

# ============================================================
# Link libraries
# ============================================================

target_link_libraries(walrus PRIVATE
    ${EGL_LIBRARIES}
    ${WALRUS_LIBRARIES}
)

# ============================================================
# Wayland dependencies
# ============================================================

if(WAYLAND_FOUND)

    target_include_directories(walrus PRIVATE
        ${WAYLAND_INCLUDE_DIRS}
    )

    target_link_directories(walrus PRIVATE
        ${WAYLAND_LIBRARY_DIRS}
    )

    target_link_libraries(walrus PRIVATE
        ${WAYLAND_LIBRARIES}
    )

endif()

# ============================================================
# X11 dependencies
# ============================================================

if(X11_FOUND)

    target_include_directories(walrus PRIVATE
        ${X11_INCLUDE_DIRS}
    )

    target_link_directories(walrus PRIVATE
        ${X11_LIBRARY_DIRS}
    )

    target_link_libraries(walrus PRIVATE
        ${X11_LIBRARIES}
    )

endif()

# ============================================================
# Compiler warnings
# ============================================================

target_compile_options(walrus PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

# ============================================================
# Examples
# ============================================================

if(WALRUS_BUILD_EXAMPLES)

    add_executable(walrus-example
        examples/hello.c
    )

    target_link_libraries(walrus-example
        PRIVATE
            walrus
    )

endif()

# ============================================================
# Installation
# ============================================================

include(GNUInstallDirs)

install(
    TARGETS walrus
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(
    DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
