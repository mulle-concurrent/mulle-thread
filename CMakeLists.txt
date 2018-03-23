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
# definitions from cmake/HeadersAndSources.cmake
${SOURCES}
${PUBLIC_HEADERS}
${PRIVATE_HEADERS}
# definitions from cmake/share/Environment.cmake
${CMAKE_EDITABLE_FILES}
)



if( BUILD_SHARED_LIBS)
   target_link_libraries( mulle-thread
# definitions from cmake/share/DependenciesAndLibraries.cmake
${DEPENDENCY_LIBRARIES}
${OS_SPECIFIC_LIBRARIES}
)
endif()

include( PostLibrary OPTIONAL)

option( STANDALONE "Create standalone library for debugging" OFF)

set( INSTALL_LIBRARY_TARGETS
"mulle-thread"
)

if( STANDALONE)
   set( STANDALONE_BASE_NAME "mulle-thread")
   set( STANDALONE_NAME "mulle-thread-standalone")
   set( STANDALONE_DEFINITIONS ${MULLE_THREAD_DEFINITIONS})
   set( STANDALONE_DEPENDENCIES
   mulle-thread
   )

   set( STANDALONE_ALL_LOAD_LIBRARIES
   $<TARGET_FILE:mulle-thread>
   ${DEPENDENCY_LIBRARIES}
   ${OPTIONAL_DEPENDENCY_LIBRARIES}
   ${OS_SPECIFIC_LIBRARIES}
   )

   include( Standalone)

   set( INSTALL_LIBRARY_TARGETS
   ${INSTALL_LIBRARY_TARGETS}
   "mulle-thread-standalone"
   )
endif()

### Install

include( PreInstall OPTIONAL)

install( TARGETS ${INSTALL_LIBRARY_TARGETS} DESTINATION "lib")
install( FILES ${PUBLIC_HEADERS}  DESTINATION "include/mulle-thread")
install( FILES ${PRIVATE_HEADERS} DESTINATION "include/mulle-thread/private")

# definitions from cmake/share/Environment.cmake
install( FILES ${CMAKE_INCLUDES}  DESTINATION "include/mulle-thread/cmake")

# definitions from cmake/share/LinkManifest.cmake
install( FILES ${LINK_MANIFEST_FILES}  DESTINATION "include/mulle-thread/link")

include( PostInstall OPTIONAL)
