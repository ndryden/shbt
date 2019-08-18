# Exports the following variables:
#
#  LIBUNWIND_FOUND
#  LIBUNWIND_INCLUDE_PATH
#  LIBUNWIND_LIBRARY
#
# Creates an imported target LIBUNWIND::libunwind

find_path(LIBUNWIND_INCLUDE_PATH libunwind.h
  HINTS ${LIBUNWIND_DIR} $ENV{LIBUNWIND_DIR}
  PATH_SUFFIXES include
  NO_DEFAULT_PATH
  )
find_path(LIBUNWIND_INCLUDE_PATH libunwind.h)

find_library(LIBUNWIND_LIBRARY unwind
  HINTS ${LIBUNWIND_DIR} $ENV{LIBUNWIND_DIR}
  PATH_SUFFIXES lib64 lib
  NO_DEFAULT_PATH
  )
find_library(LIBUNWIND_LIBRARY unwind)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
  LIBUNWIND DEFAULT_MSG LIBUNWIND_LIBRARY LIBUNWIND_INCLUDE_PATH)

if (NOT TARGET LIBUNWIND::libunwind)
  add_library(LIBUNWIND::libunwind INTERFACE IMPORTED)
  set_property(TARGET LIBUNWIND::libunwind PROPERTY
    INTERFACE_INCLUDE_DIRECTORIES "${LIBUNWIND_INCLUDE_PATH}")
  set_property(TARGET LIBUNWIND::libunwind PROPERTY
    INTERFACE_LINK_LIBRARIES "${LIBUNWIND_LIBRARY}")
endif()
