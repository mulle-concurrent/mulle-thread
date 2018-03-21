cmake_minimum_required( VERSION 3.0)

project( mulle-thread C)


### mulle-sde environment

# add cmake module paths to search path
list( INSERT CMAKE_MODULE_PATH 0 "${PROJECT_SOURCE_DIR}/cmake/share")
list( INSERT CMAKE_MODULE_PATH 0 "${PROJECT_SOURCE_DIR}/cmake")

include( Environment)


### Files

include( PreFiles OPTIONAL)

set( MULLE_THREAD_LIBRARY ON)

#
# Include all non underscored .cmake files in ./cmake
#
file( GLOB LOCAL_CMAKE_INCLUDES
   "cmake/[A-Za-z0-9]*.cmake"
)

foreach( FILE ${LOCAL_CMAKE_INCLUDES} )
   include( "${FILE}")
endforeach()


include_directories( ${INCLUDE_DIRS})

include( PostFiles OPTIONAL)

### Library

include( PreLibrary OPTIONAL)

add_library( "mulle-thread"
${SOURCES}
${PUBLIC_HEADERS}
${PRIVATE_HEADERS}
${CMAKE_EDITABLE_FILES}
)

if( BUILD_SHARED_LIBS)
   target_link_libraries( mulle-thread
${DEPENDENCIES_LIBRARIES}
${OS_SPECIFIC_LIBRARIES}
)
endif()

include( PostLibrary OPTIONAL)


set( STANDALONE_BASE_NAME "mulle-thread")
set( STANDALONE_NAME "mulle-thread-standalone")
set( STANDALONE_DEFINITIONS ${MULLE_THREAD_DEFINITIONS})
set( STANDALONE_DEPENDENCIES
mulle-thread
)

set( STANDALONE_ALL_LOAD_LIBRARIES
$<TARGET_FILE:mulle-thread>
${C_DEPENDENCY_LIBRARIES}
${OS_SPECIFIC_LIBRARIES}
)

include( Standalone)

### Install

include( PreInstall OPTIONAL)

install( TARGETS "mulle-thread" "mulle-thread-standalone" DESTINATION "lib")
install( FILES ${PUBLIC_HEADERS}  DESTINATION "include/mulle-thread")
install( FILES ${PRIVATE_HEADERS} DESTINATION "include/mulle-thread/private")

install( FILES ${CMAKE_INCLUDES}  DESTINATION "include/mulle-thread/cmake")

include( PostInstall OPTIONAL)
