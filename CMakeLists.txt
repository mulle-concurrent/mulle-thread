cmake_minimum_required( VERSION 3.14)

project( mulle-thread VERSION 4.6.1 LANGUAGES C)




### mulle-sde environment

# add cmake module paths to search path
list( INSERT CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_SOURCE_DIR}/cmake/share")
list( INSERT CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_SOURCE_DIR}/cmake/reflect")
list( INSERT CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include( Environment)

include( Files)

### Library

if( HEADERS_PHASE)
   install( FILES ${INSTALL_PUBLIC_HEADERS}  DESTINATION "include/mulle-thread")
   install( FILES ${INSTALL_PRIVATE_HEADERS} DESTINATION "include/mulle-thread")
   install( FILES ${INSTALL_CMAKE_INCLUDES}  DESTINATION "include/mulle-thread/cmake")
   # manually maintained!
   install( FILES ${MINTOMIC_PUBLIC_HEADERS}  DESTINATION "include/mintomic")
   # NOTE: it's really private subdir!
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

# use LIBRARY_COMPILE_TARGET for compiler options
target_compile_definitions( "${LIBRARY_COMPILE_TARGET}" PUBLIC)
target_compile_options( "${LIBRARY_COMPILE_TARGET}" PUBLIC)
if( NOT MSVC)
   target_compile_options( "${LIBRARY_COMPILE_TARGET}" PUBLIC -Wunused)
endif()

# use LIBRARY_LINK_TARGET for linker options
if( LINK_PHASE)
   target_link_options( "${LIBRARY_LINK_TARGET}" PUBLIC)
endif()

include( InstallLibrary)

if( LINK_PHASE)
   include( InstallCMakeInclude)
   include( InstallCMakePackage)
endif() 

include( FinalOutput OPTIONAL)
