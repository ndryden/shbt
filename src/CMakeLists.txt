set_full_path(THIS_DIR_SOURCES
  shbt_signal.c
  shbt_backtrace.c
  shbt_utils.c
  demangle_ia64.c
  demangle_abi.cpp
  )

set(SHBT_SOURCES "${SOURCES}" "${THIS_DIR_SOURCES}" PARENT_SCOPE)
