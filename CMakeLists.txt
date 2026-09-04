cmake_minimum_required( VERSION 3.13...99.99)

project( mulle-thread VERSION 4.10.0 LANGUAGES C)




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

#
# The shared Library.cmake defines "${LIBRARY_UPCASE_IDENTIFIER}_BUILD"
# (single underscore) on the compile target, but the mulle convention used
# by src/generic/include.h is the double underscore variant
# "MULLE__THREAD_BUILD" (cf. MULLE__CORE_BUILD, MULLE__C11_VERSION).
# Define it here so that MULLE__THREAD_GLOBAL resolves to MULLE_C_GLOBAL
# while compiling the library (visibility / dllexport on Windows).
#
target_compile_definitions( "${LIBRARY_COMPILE_TARGET}" PRIVATE MULLE__THREAD_BUILD)

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
