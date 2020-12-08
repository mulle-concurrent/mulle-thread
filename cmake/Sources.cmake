#
# The following includes include definitions generated
# during `mulle-sde update`. Don't edit those files. They are
# overwritten frequently.
#
# === MULLE-SDE START ===

include( _Sources)

# === MULLE-SDE END ===
#
#
# If you don't like the "automatic" way of generating _Sources
#
# MULLE_MATCH_TO_CMAKE_SOURCES_FILE="DISABLE" # or NONE
#

#
# You can put more source and resource file definitions here.
#

set( SOURCES
src/mulle-thread-linkage.c
)

set( STANDALONE_SOURCES
src/mulle-thread-standalone.c
)


if( MSVC)
   set( SOURCES
${SOURCES}
src/mulle-thread-windows.c
)
endif()

