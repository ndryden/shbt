set_full_path(THIS_DIR_HEADERS
  shbt.h
  shbt_internal.h
  )

set(SHBT_HEADERS "${HEADERS}" "${THIS_DIR_HEADERS}" PARENT_SCOPE)
