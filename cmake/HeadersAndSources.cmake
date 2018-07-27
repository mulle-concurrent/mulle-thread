#
# The following includes include definitions generated
# during `mulle-sde update`. Don't edit those files. They are
# overwritten frequently.
#
# === MULLE-SDE START ===

include( _Headers)
include( _Sources)

# === MULLE-SDE END ===
#

# add ignored header back in
# add ignored headers back in
set( PUBLIC_HEADERS
"src/_mulle-thread-include.h"
${PUBLIC_HEADERS}
)

set( PRIVATE_HEADERS
"src/_mulle-thread-include-private.h"
${PRIVATE_HEADERS}
)

set( SOURCES
src/linkage.c
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


set( MINTOMIC_HEADERS
mintomic/include/mintomic/core.h
mintomic/include/mintomic/mintomic.h
mintomic/include/mintomic/platform_detect.h)


set( MINTOMIC_PRIVATE_HEADERS
mintomic/include/mintomic/private/core_gcc.h
mintomic/include/mintomic/private/core_msvc.h
mintomic/include/mintomic/private/mintomic_gcc_arm.h
mintomic/include/mintomic/private/mintomic_gcc_x86-64.h
mintomic/include/mintomic/private/mintomic_msvc.h
mintomic/include/mintomic/private/mintomic_stdint.h
)

include_directories(
AFTER SYSTEM mintomic/include
)

#
# You can put more source and resource file definitions here.
#
