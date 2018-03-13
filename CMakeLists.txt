cmake_minimum_required( VERSION 3.0)

project( mulle-thread C)


### mulle-sde environment

# add cmake module paths to search path
list( INSERT CMAKE_MODULE_PATH 0 "${PROJECT_SOURCE_DIR}/.mulle-sde/share/cmake")
list( INSERT CMAKE_MODULE_PATH 0 "${PROJECT_SOURCE_DIR}/.mulle-sde/etc/cmake")
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

include_directories( AFTER SYSTEM mintomic/include)
target_include_directories( mulle-thread PUBLIC mintomic/include)

include( PostLibrary OPTIONAL)

### Install

include( PreInstall OPTIONAL)

install( TARGETS "mulle-thread" DESTINATION "lib")
install( FILES ${PUBLIC_HEADERS}  DESTINATION "include/mulle-thread")
install( FILES ${PRIVATE_HEADERS} DESTINATION "include/mulle-thread/private")
install( FILES ${CMAKE_INCLUDES}  DESTINATION "include/mulle-thread/cmake")

install( FILES ${MINTOMIC_HEADERS} DESTINATION "include/mintomic")
install( FILES ${MINTOMIC_PRIVATE_HEADERS} DESTINATION "include/mintomic/private")

include( PostInstall OPTIONAL)
