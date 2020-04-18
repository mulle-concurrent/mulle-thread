#
# The following includes include definitions generated
# during `mulle-sde update`. Don't edit those files. They are
# overwritten frequently.
#
# === MULLE-SDE START ===

include( _Headers)

# === MULLE-SDE END ===
#

#
# If you don't like the "automatic" way of generating _Headers
#
# MULLE_MATCH_TO_CMAKE_HEADERS_FILE="DISABLE" # or NONE
#

#
# Add ignored headers back in so that the generators pick them up
#
set( PUBLIC_HEADERS
"src/reflect/_mulle-thread-include.h"
${PUBLIC_HEADERS}
)

# keep headers to install separate to make last minute changes
set( INSTALL_PUBLIC_HEADERS ${PUBLIC_HEADERS})

#
# Do not install generated private headers and include-private.h
# which aren't valid outside of the project scope.
#
set( INSTALL_PRIVATE_HEADERS ${PRIVATE_HEADERS})
list( REMOVE_ITEM INSTALL_PRIVATE_HEADERS "src/include-private.h")

# add ignored headers back in so that the generators pick them up
set( PRIVATE_HEADERS
"src/reflect/_mulle-thread-include-private.h"
${PRIVATE_HEADERS}
)

#
# You can put more source and resource file definitions here.
#
# add ignored header back in
# add ignored headers back in

set( MINTOMIC_PUBLIC_HEADERS
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

