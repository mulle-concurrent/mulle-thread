if( MULLE_TRACE_INCLUDE)
   message( STATUS "# Include \"${CMAKE_CURRENT_LIST_FILE}\"" )
endif()

include( CompilerDetection)
include( UnwantedWarnings)

include( PreLibraryCAux OPTIONAL)
