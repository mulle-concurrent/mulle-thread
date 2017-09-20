cmake_minimum_required( VERSION 3.0)

project( mulle-thread C)


set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/mulle-configuration/cmake)

message( STATUS "CMAKE_MODULE_PATH is ${CMAKE_MODULE_PATH}")
message( STATUS "CMAKE_BUILD_TYPE is ${CMAKE_BUILD_TYPE}")

if( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
  include( Debug)
else()
  include( Release)
endif()


set( MULLE_THREAD_LIBRARY ON)
include( CMakeSourcesAndHeaders.txt)
include( CMakeDependencies.txt)


set( PUBLIC_HEADERS
${HEADERS}
CMakeDependencies.txt
)


add_library( mulle_thread
${SOURCES}  # for IDEs
${HEADERS}  # for IDEs
${MINTOMIC_HEADERS}  # for IDEs
${MINTOMIC_PRIVATE_HEADERS}  # for IDEs
CMakeLists.txt
CMakeSourcesAndHeaders.txt
)


include_directories( AFTER SYSTEM mintomic/include)
target_include_directories( mulle_thread PUBLIC mintomic/include)

link_directories( ${CMAKE_BINARY_DIR})
target_link_libraries( mulle_thread LINK_PUBLIC
${C_DEPENDENCY_LIBRARIES}
${OS_SPECIFIC_LIBRARIES}
)

INSTALL( TARGETS mulle_thread DESTINATION "lib")
INSTALL( FILES ${PUBLIC_HEADERS} DESTINATION "include/mulle_thread")

INSTALL( FILES ${MINTOMIC_HEADERS} DESTINATION "include/mintomic")
INSTALL( FILES ${MINTOMIC_PRIVATE_HEADERS} DESTINATION "include/mintomic/private")

include( MulleTest)

