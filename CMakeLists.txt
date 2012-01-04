cmake_minimum_required( VERSION 2.8 )

if( NOT CMAKE_BUILD_TYPE )
	set( CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build (Debug or Release)" FORCE )
endif()

project( FlexWorld )

set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake" )

#
# Detections.
#
if( ${CMAKE_SYSTEM_NAME} MATCHES "Windows" )
	set( WINDOWS 1 )
elseif( ${CMAKE_SYSTEM_NAME} MATCHES "Linux" )
	set( LINUX 1 )
else()
	message( FATAL_ERROR "Unsupported system." )
endif()

if( CMAKE_COMPILER_IS_GNUCXX )
	set( COMPILER_GCC 1 )
elseif( MSVC_VERSION )
	set( COMPILER_MSVC 1 )
endif()

#
# Global project settings.
#
set( FW_VERSION_SUFFIX "DEV" CACHE STRING "Version suffix." )
set( FW_PORTABLE_INSTALL TRUE CACHE BOOL "Install portable (especially useful for distributing Linux test releases(!), so that both client and server don't use absolute system paths)." )

# Configure data root directory.
if( WINDOWS )
	set( FW_ROOT_DATA_DIRECTORY "./" )
else()
	if( FW_PORTABLE_INSTALL )
		set( FW_ROOT_DATA_DIRECTORY "./" )
	else()
		set( FW_ROOT_DATA_DIRECTORY "${CMAKE_INSTALL_PREFIX}/games/flexworld/" )
	endif()
endif()

#
# Targets.
#
set( FW_BUILD_CLIENT true CACHE BOOL "Build client." )
set( FW_BUILD_SERVER true CACHE BOOL "Build (dedicated) server." )
set( FW_BUILD_TEST true CACHE BOOL "Build unit tests." )
set( FW_BUILD_DOC false CACHE BOOL "Build API docs (requires Doxygen)." )

#
# Platform-specific.
#
if( WINDOWS )
	set( BIN_EXTENSION ".exe" )
	set( SFML_STATIC false CACHE BOOL "Was SFML built as a static library?" )
	set( SFGUI_STATIC false CACHE BOOL "Was SFGUI built as a static library?" )
	set( SHARE_TARGET_DIR . )
	add_definitions( -DWINDOWS )
else()
	set( SHARE_TARGET_DIR share/FlexWorld )
	add_definitions( -DLINUX )
endif()

#
# Linkage.
#
if( SFGUI_STATIC )
	add_definitions( -DSFGUI_STATIC )
endif()

if( SFML_STATIC )
	add_definitions( -DSFML_STATIC_LIBRARIES )
endif()

#
# Compiler-specific.
#
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

add_subdirectory( "lib" )

if( FW_BUILD_DOC )
	add_subdirectory( "doc" )
endif()

if( FW_BUILD_SERVER )
	add_subdirectory( "server" )
endif()

if( FW_BUILD_CLIENT )
	add_subdirectory( "client" )
endif()

if( FW_BUILD_TEST )
	add_subdirectory( "test" )
endif()

add_subdirectory( "modes" )
