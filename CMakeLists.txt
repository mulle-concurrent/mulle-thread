cmake_minimum_required( VERSION 3.1)

project( mulle-thread C)


### mulle-sde environment

# add cmake module paths to search path
list( INSERT CMAKE_MODULE_PATH 0 "${PROJECT_SOURCE_DIR}/cmake/share")
list( INSERT CMAKE_MODULE_PATH 0 "${PROJECT_SOURCE_DIR}/cmake/reflect")
list( INSERT CMAKE_MODULE_PATH 0 "${PROJECT_SOURCE_DIR}/cmake")

include( Environment)

include( Files)

### Library

if( HEADERS_PHASE)
   install( FILES ${INSTALL_PUBLIC_HEADERS}  DESTINATION "include/mulle-thread")
   install( FILES ${INSTALL_PRIVATE_HEADERS} DESTINATION "include/mulle-thread/private")
   install( FILES ${CMAKE_INCLUDES}          DESTINATION "include/mulle-thread/cmake")
   # manually maintained!
   install( FILES ${MINTOMIC_PUBLIC_HEADERS}  DESTINATION "include/mintomic")
   install( FILES ${MINTOMIC_PRIVATE_HEADERS} DESTINATION "include/mintomic/private")

   # short cut out
   if( NOT COMPILE_PHASE AND NOT LINK_PHASE)
      return()
   endif()
endif()

if( LINK_PHASE)
   include( Dependencies OPTIONAL)
endif()

include( Library)
include( InstallLibrary)
include( FinalOutput OPTIONAL)
