cmake_minimum_required (VERSION 3.0)

project (mulle-thread)

#
# dont forget to wipe all cmake produced files before
# changing this
#

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
   set (CMAKE_C_FLAGS "-Wno-parantheses")
endif()

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -g -DDEBUG -DMULLE_ATOMIC_TRACE")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -g -DNDEBUG")

set(HEADERS
src/mulle_atomic.h
src/mulle_atomic_mintomic.h
src/mulle_thread.h
src/mulle_thread_pthreads.h)

set(MINTOMIC_HEADERS
mulle_mintomic/include/mintomic/core.h
mulle_mintomic/include/mintomic/mintomic.h
mulle_mintomic/include/mintomic/platform_detect.h)

set(MINTOMIC_PRIVATE_HEADERS
mulle_mintomic/include/mintomic/private/core_gcc.h
mulle_mintomic/include/mintomic/private/core_msvc.h
mulle_mintomic/include/mintomic/private/mintomic_gcc_arm.h
mulle_mintomic/include/mintomic/private/mintomic_gcc_x86-64.h
mulle_mintomic/include/mintomic/private/mintomic_msvc.h
mulle_mintomic/include/mintomic/private/stdint.h
)

add_library(mulle_thread
src/linkage.c
mulle_mintomic/src/mintomic/mintomic_gcc.c
)


include_directories( AFTER SYSTEM mintomic/include)
target_include_directories( mulle_thread PUBLIC mulle_mintomic/include)

link_directories(${CMAKE_BINARY_DIR})
target_link_libraries( mulle_thread LINK_PUBLIC pthread)

INSTALL(TARGETS mulle_thread DESTINATION "lib")

INSTALL(FILES ${HEADERS} DESTINATION "include/mulle_thread")

# to avoid conflict with another mintomic installation
# use mulle_mintomic as include subdir

INSTALL(FILES ${MINTOMIC_HEADERS} DESTINATION "include/mulle_mintomic")
INSTALL(FILES ${MINTOMIC_PRIVATE_HEADERS} DESTINATION "include/mulle_mintomic/private")
