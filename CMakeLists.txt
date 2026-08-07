cmake_minimum_required(VERSION 3.20)

project(Walrus
  VERSION 0.1.0
  DESCRIPTION "A modern C GUI toolkit for Linux using Wayland and X11"
  LANGUAGES C
)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Options
option(WALRUS_BUILD_EXAMPLES "Build examples" ON)
option(WALRUS_ENABLE_WAYLAND "Enable Wayland backend" ON)
option(WALRUS_ENABLE_X11 "Enable X11 backend" ON)

# Library sources
set(WALRUS_SOURCES
  src/walrus.c
  src/window.c
  src/backend/backend.c
)

# Dependencies
find_package(PkgConfig REQUIRED)

if(WALRUS_ENABLE_WAYLAND)
  pkg_check_modules(WAYLAND wayland-client)

  if(WAYLAND_FOUND)
    message(STATUS "Wayland backend enabled")

    list(APPEND WALRUS_SOURCES
      src/backend/wayland/wayland.c
      src/backend/wayland/xdg-shell-protocol.c
    )
  else()
    message(WARNING "Wayland not found")
  endif()
endif()

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


# Main library
add_library(walrus SHARED
  ${WALRUS_SOURCES}
)

target_include_directories(walrus
  PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

# Link dependencies

if(WAYLAND_FOUND)
  target_include_directories(walrus PRIVATE
    ${WAYLAND_INCLUDE_DIRS}
  )

  target_link_libraries(walrus
    PRIVATE
      ${WAYLAND_LIBRARIES}
  )
endif()

if(X11_FOUND)
  target_include_directories(walrus PRIVATE
    ${X11_INCLUDE_DIRS}
  )

  target_link_libraries(walrus
    PRIVATE
      ${X11_LIBRARIES}
  )
endif()


# Compiler warnings
target_compile_options(walrus PRIVATE
  -Wall
  -Wextra
  -Wpedantic
)

target_include_directories(walrus
  PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

# Examples
if(WALRUS_BUILD_EXAMPLES)

  add_executable(walrus-example
    examples/hello.c
  )

  target_link_libraries(walrus-example
    PRIVATE
      walrus
  )

endif()


# Installation
include(GNUInstallDirs)

install(
  TARGETS walrus
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(
  DIRECTORY include/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
