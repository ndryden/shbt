cmake_minimum_required(VERSION 3.12)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

set(SHBT_VERSION_MAJOR 0)
set(SHBT_VERSION_MINOR 1)
set(SHBT_VERSION_PATCH 0)
set(SHBT_VERSION "${SHBT_VERSION_MAJOR}.${SHBT_VERSION_MINOR}.${SHBT_VERSION_PATCH}")

project(SHBT VERSION ${SHBT_VERSION} LANGUAGES C CXX)

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "CMake build type" FORCE)
endif ()

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED TRUE)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC -g -Wall -Wextra")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0")
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -g -Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")

include(GNUInstallDirs)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

find_package(Libunwind REQUIRED)

# Options.
option(SHBT_ENABLE_MPI "Enable MPI support." OFF)
if (SHBT_ENABLE_MPI)
  include(SetupMPI)
  set(SHBT_HAVE_MPI TRUE)
endif ()

set(SHBT_DEMANGLER BUILTIN_IA64 CACHE STRING "Select C++ symbol demangler")
set_property(CACHE SHBT_DEMANGLER PROPERTY STRINGS BUILTIN_IA64 ABI)
if (SHBT_DEMANGLER STREQUAL "BUILTIN_IA64")
  set(SHBT_USE_BUILTIN_IA64_DEMANGLER TRUE)
elseif (SHBT_DEMANGLER STREQUAL "ABI")
  set(SHBT_USE_ABI_DEMANGLER TRUE)
else ()
  message(FATAL_ERROR "Unknown demangler")
endif()

configure_file(
  "${CMAKE_SOURCE_DIR}/cmake/configure_files/shbt_config.h.in"
  "${CMAKE_BINARY_DIR}/shbt_config.h"
  @ONLY)

include(SHBTCMakeUtilities)
add_subdirectory(include)
add_subdirectory(src)
add_subdirectory(test)

add_library(shbt SHARED ${SHBT_SOURCES} ${SHBT_HEADERS})
set_target_properties(shbt PROPERTIES VERSION ${SHBT_VERSION})
set_target_properties(shbt PROPERTIES SOVERSION ${SHBT_VERSION_MAJOR})

target_include_directories(shbt PUBLIC
  $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}>)

target_link_libraries(shbt PUBLIC LIBUNWIND::libunwind)

if (SHBT_HAVE_MPI)
  target_link_libraries(shbt PUBLIC MPI::MPI_C)
endif ()

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
  "${CMAKE_BINARY_DIR}/SHBTConfigVersion.cmake" VERSION
  ${SHBT_VERSION} COMPATIBILITY SameMinorVersion)

install(
  TARGETS shbt
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  )

install(
  DIRECTORY "${PROJECT_SOURCE_DIR}/include/shbt"
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  FILES_MATCHING PATTERN "*.h"
  )

install(
  FILES "${CMAKE_BINARY_DIR}/shbt_config.h"
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  )
install(
  FILES "${CMAKE_BINARY_DIR}/SHBTConfigVersion.cmake"
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/shbt
  )
