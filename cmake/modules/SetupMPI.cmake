find_package(MPI REQUIRED)

if (MPI_C_FOUND)
  if (NOT TARGET MPI::MPI_C)
    add_library(MPI::MPI_C INTERFACE IMPORTED)
    if (MPI_C_COMPILE_FLAGS)
      separate_arguments(_MPI_C_COMPILE_OPTIONS UNIX_COMMAND
        "${MPI_C_COMPILE_FLAGS}")
      set_property(TARGET MPI::MPI_C PROPERTY
        INTERFACE_COMPILE_OPTIONS "${_MPI_C_COMPILE_OPTIONS}")
    endif()

    if (MPI_C_LINK_FLAGS)
      separate_arguments(_MPI_C_LINK_LINE UNIX_COMMAND
        "${MPI_C_LINK_FLAGS}")
    endif()
    list(APPEND _MPI_C_LINK_LINE "${MPI_C_LIBRARIES}")

    set_property(TARGET MPI::MPI_C PROPERTY
      INTERFACE_LINK_LIBRARIES "${_MPI_C_LINK_LINE}")

    set_property(TARGET MPI::MPI_C PROPERTY
      INTERFACE_INCLUDE_DIRECTORIES "${MPI_C_INCLUDE_PATH}")

  endif (NOT TARGET MPI::MPI_C)
else ()
  message(FATAL_ERROR "MPI C compiler was not found and is required")
endif (MPI_C_FOUND)
