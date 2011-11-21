cmake_minimum_required( VERSION 2.8 )

if( NOT CMAKE_BUILD_TYPE )
	set( CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build (Debug or Release)" FORCE )
endif()

project( FlexWorld )

find_package( SFML 2.0 REQUIRED GRAPHICS WINDOW SYSTEM )
find_package( SFGUI REQUIRED )
find_package( OpenGL REQUIRED )

if( ${CMAKE_SYSTEM_NAME} MATCHES "Windows" )
	set( WINDOWS 1 )
endif()

if( CMAKE_COMPILER_IS_GNUCXX )
	set( COMPILER_GCC 1 )
elseif( MSVC_VERSION )
	set( COMPILER_MSVC 1 )
endif()

set( FW_BUILD_CLIENT true CACHE BOOL "Build client." )
set( FW_BUILD_SERVER true CACHE BOOL "Build (dedicated) server." )

if( WINDOWS )
	set( BIN_EXTENSION ".exe" )
	set( SFML_STATIC false CACHE BOOL "Was SFML built as a static library?" )
	set( SFGUI_STATIC false CACHE BOOL "Was SFGUI built as a static library?" )
	set( SHARE_TARGET_DIR . )
else()
	set( SHARE_TARGET_DIR share/FlexWorld )
endif()

if( SFGUI_STATIC )
	add_definitions( -D SFGUI_STATIC )
endif()

if( SFML_STATIC )
	add_definitions( -D SFML_STATIC )
endif()

if( COMPILER_GCC )
	if( NOT CMAKE_CXX_FLAGS )
		set( CMAKE_CXX_FLAGS "--std=c++0x -Wall -Wextra -Wshadow -Wconversion -pedantic" CACHE STRING "C++ compiler flags" FORCE )
		set( CMAKE_C_FLAGS "--std=c++0x -Wall -Wextra -Wshadow -Wconversion -pedantic" CACHE STRING "C compiler flags" FORCE )
	endif()

	if( NOT CMAKE_CXX_FLAGS_DEBUG )
		set( CMAKE_CXX_FLAGS_DEBUG "-g -O0" CACHE STRING "C++ compiler flags (debug)" FORCE )
		set( CMAKE_C_FLAGS_DEBUG "-g -O0" CACHE STRING "C++ compiler flags (debug)" FORCE )
	endif()
endif()

#include_directories( ${INC_DIR} )
#include_directories( ${SFML_INCLUDE_DIR} )
#include_directories( ${OPENGL_INCLUDE_DIR} )

#if( NOT SFGUI_STATIC )
#add_library( sfgui SHARED ${SOURCES} )

#set_target_properties( sfgui PROPERTIES DEBUG_POSTFIX -d )

#if ( WINDOWS AND COMPILER_GCC )
#set_target_properties( sfgui PROPERTIES PREFIX "" )
#set_target_properties( sfgui PROPERTIES IMPORT_SUFFIX ".a" )
#endif()
#else()
#add_library( sfgui ${SOURCES} )

#set_target_properties( sfgui PROPERTIES DEBUG_POSTFIX -s-d )
#set_target_properties( sfgui PROPERTIES RELEASE_POSTFIX -s )
#endif()

#set_target_properties( sfgui PROPERTIES COMPILE_DEFINITIONS SFGUI_EXPORTS )

#target_link_libraries( sfgui ${SFML_GRAPHICS_LIBRARY} ${SFML_WINDOW_LIBRARY} ${SFML_SYSTEM_LIBRARY} ${OPENGL_gl_LIBRARY} )

add_subdirectory( "lib" )
add_subdirectory( "test" )
add_subdirectory( "server" )

#if( FW_BUILD_CLIENT )
#add_subdirectory( "fwclient" )
#endif()

#if( SFGUI_BUILD_EXAMPLES )
#add_subdirectory( "examples" )
#endif()

### INSTALL TARGETS ###

#install(
#	TARGETS sfgui
#	RUNTIME DESTINATION bin COMPONENT bin
#	LIBRARY DESTINATION lib COMPONENT bin
#	ARCHIVE DESTINATION lib COMPONENT dev
#)
#
#install(
#	DIRECTORY include
#	DESTINATION .
#)
#
#install(
#	FILES README AUTHORS LICENSE
#	DESTINATION ${SHARE_TARGET_DIR}
#)
