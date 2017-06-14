cmake_minimum_required (VERSION 3.0)

project (mulle-thread)

cmake_policy(SET CMP0054 NEW)

set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/mulle-configuration/cmake)

message(STATUS "CMAKE_MODULE_PATH is ${CMAKE_MODULE_PATH}")
message(STATUS "CMAKE_BUILD_TYPE is ${CMAKE_BUILD_TYPE}")

if( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
  include( Debug)
else()
  include( Release)
endif()


set(HEADERS
src/mulle_atomic.h
src/mulle_atomic_mintomic.h
src/mulle_atomic_c11.h
src/mulle_thread.h
src/mulle_thread_c11.h
src/mulle_thread_pthreads.h
src/mulle_thread_windows.h
)

set(MINTOMIC_HEADERS
mintomic/include/mintomic/core.h
mintomic/include/mintomic/mintomic.h
mintomic/include/mintomic/platform_detect.h)

set(MINTOMIC_PRIVATE_HEADERS
mintomic/include/mintomic/private/core_gcc.h
mintomic/include/mintomic/private/core_msvc.h
mintomic/include/mintomic/private/mintomic_gcc_arm.h
mintomic/include/mintomic/private/mintomic_gcc_x86-64.h
mintomic/include/mintomic/private/mintomic_msvc.h
mintomic/include/mintomic/private/mintomic_stdint.h
)

add_library( mulle_thread
src/linkage.c
src/mulle_thread_windows.c
${HEADERS}  # for IDEs
${MINTOMIC_HEADERS}  # for IDEs
${MINTOMIC_PRIVATE_HEADERS}  # for IDEs
)


include_directories( AFTER SYSTEM mintomic/include)
target_include_directories( mulle_thread PUBLIC mintomic/include)

link_directories(${CMAKE_BINARY_DIR})
target_link_libraries( mulle_thread LINK_PUBLIC pthread)

INSTALL(TARGETS mulle_thread DESTINATION "lib")
INSTALL(FILES ${HEADERS} DESTINATION "include/mulle_thread")

INSTALL(FILES ${MINTOMIC_HEADERS} DESTINATION "include/mintomic")
INSTALL(FILES ${MINTOMIC_PRIVATE_HEADERS} DESTINATION "include/mintomic/private")
