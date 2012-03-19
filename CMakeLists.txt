cmake_minimum_required( VERSION 2.8 )
project( FlexWorld )

if( NOT CMAKE_BUILD_TYPE )
	set( CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build (Debug or Release)" FORCE )
endif()

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

set( CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${PROJECT_SOURCE_DIR}/cmake;${PROJECT_SOURCE_DIR}/extlibs/SFML/cmake/Modules;${PROJECT_SOURCE_DIR}/extlibs/SFGUI/cmake" )

#
# Global project settings.
#
set( FW_VERSION_MAJOR "0" )
set( FW_VERSION_MINOR "0" )
set( FW_VERSION_REVISION "0" )
set( FW_VERSION_SUFFIX "12-11-0" )

set( FW_PORTABLE_INSTALL TRUE CACHE BOOL "Install portable (especially useful for distributing Linux test releases(!), so that both client and server don't use absolute system paths)." )

# Configure data root directory.
if( FW_PORTABLE_INSTALL OR WINDOWS )
	set( FW_ROOT_DATA_DIRECTORY "./" )
else()
	set( FW_ROOT_DATA_DIRECTORY "${CMAKE_INSTALL_PREFIX}/share/games/flexworld/" )
endif()

#
# Properties.
#
set( FW_BUILD_CLIENT true CACHE BOOL "Build client." )
set( FW_BUILD_SERVER true CACHE BOOL "Build (dedicated) server." )
set( FW_BUILD_TEST true CACHE BOOL "Build unit tests." )
set( FW_BUILD_DOC false CACHE BOOL "Build API docs (requires Doxygen)." )
set( FW_BUILD_CONVERT2FWM false CACHE BOOL "Build convert2fwm tool." )
set( FW_BUILD_DEB false CACHE BOOL "Make Debian package." )

#
# Platform-specific.
#
if( WINDOWS )
	set( BIN_EXTENSION ".exe" )
	set( SHARE_TARGET_DIR . )
	add_definitions( -DWINDOWS )
else()
	set( SHARE_TARGET_DIR share/FlexWorld )
	add_definitions( -DLINUX )
endif()

#
# Linker options.
#
set( Boost_USE_STATIC_LIBS false CACHE BOOL "Use Boost's static libraries." )
set( SFML_STATIC_LIBRARIES false CACHE BOOL "Was SFML built as a static library?" )
set( SFGUI_STATIC_LIBRARIES false CACHE BOOL "Was SFGUI built as a static library?" )

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

#
# Boost-specific.
#
add_definitions( -DBOOST_DATE_TIME_NO_LIB )
add_definitions( -DBOOST_REGEX_NO_LIB )
set( Boost_ADDITIONAL_VERSIONS "1.49" "1.49.0" )

if( WINDOWS )
	add_definitions( -D_WIN32_WINNT=0x0501 )
endif()

#
# Find packages.
#
set( BOOST_COMPONENTS thread filesystem system )
set( SFML_COMPONENTS SYSTEM )

if( FW_BUILD_CLIENT )
	set( BOOST_COMPONENTS ${BOOST_COMPONENTS} date_time )
	set( SFML_COMPONENTS ${SFML_COMPONENTS} GRAPHICS AUDIO WINDOW )
endif()

if( FW_BUILD_TEST )
	set( BOOST_COMPONENTS ${BOOST_COMPONENTS} unit_test_framework )
endif()

find_package( Boost 1.47 REQUIRED COMPONENTS ${BOOST_COMPONENTS} )
find_package( SFML 2.0 REQUIRED COMPONENTS ${SFML_COMPONENTS} )
find_package( yaml-cpp REQUIRED )
find_package( Threads REQUIRED )

if( NOT WINDOWS )
	find_package( X11 REQUIRED )
endif()

if( FW_BUILD_CLIENT )
	find_package( OpenGL REQUIRED )
	find_package( SFGUI REQUIRED )
endif()

#
# BUILD.
#

# FWSG.
set( FWSG_SKIP_INSTALL TRUE )
add_subdirectory( "extlibs/FWSG" )

# FlexWorld common library.
add_subdirectory( "lib" )

# Doxygen.
if( FW_BUILD_DOC )
	add_subdirectory( "doc" )
endif()

# Dedicated server.
if( FW_BUILD_SERVER )
	add_subdirectory( "server" )
endif()

# Client.
if( FW_BUILD_CLIENT )
	add_subdirectory( "client" )
endif()

# Unittests.
if( FW_BUILD_TEST )
	add_subdirectory( "test" )
endif()

# Convert2FWM tool.
if( FW_BUILD_CONVERT2FWM )
	add_subdirectory( "tools/convert2fwm" )
endif()

# Process/install game modes.
add_subdirectory( "modes" )

#
# Setup CPack.
#
set( CPACK_PACKAGE_NAME "flexworld" )
set( CPACK_PACKAGE_VENDOR "BoxBox.org" )
set( CPACK_PACKAGE_VERSION_MAJOR "${FW_VERSION_MAJOR}" )
set( CPACK_PACKAGE_VERSION_MINOR "${FW_VERSION_MINOR}" )
set( CPACK_PACKAGE_VERSION_PATCH "${FW_VERSION_REVISION}" )
set( CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}" )

if( FW_VERSION_SUFFIX )
	set( CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION}-${FW_VERSION_SUFFIX}" )
endif()

set( CPACK_PACKAGE_DESCRIPTION "Flexible 3D multiplayer sandbox game." )

if( WINDOWS )
else()
endif()

if( CMAKE_SIZEOF_VOID_P MATCHES "8" )
	set( CPACK_SYSTEM_NAME "amd64" )
else()
	set( CPACK_SYSTEM_NAME "i386" )
endif()

set( CPACK_TOPLEVEL_TAG "${CPACK_SYSTEM_NAME}" )

# Build Debian package.
if( FW_BUILD_DEB )
	if( FW_PORTABLE_INSTALL )
		message( FATAL_ERROR "Can't make a Debian package with portable install." )
	endif()

	# Include menu shortcut.
	INSTALL(
		FILES ${PROJECT_SOURCE_DIR}/dist/linux/flexworld.desktop
		DESTINATION share/applications/
	)

	set( CPACK_GENERATOR "DEB" )
	set( CPACK_DEBIAN_PACKAGE_MAINTAINER "Stefan Schindler <support@flexworld-game.com>" )
	set( CPACK_DEBIAN_PACKAGE_NAME "${CPACK_PACKAGE_NAME}" )
	set( CPACK_DEBIAN_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION}" )
	set( CPACK_DEBIAN_PACKAGE_ARCHITECTURE "${CPACK_SYSTEM_NAME}" )
	set( CPACK_DEBIAN_PACKAGE_DEPENDS "libjpeg8, libxrandr2, libglew1.6, libopenal1, libsndfile1, libpng12-0, zlib1g, libsoil1, libfreetype6" )
	set( CPACK_DEBIAN_PACKAGE_DESCRIPTION "${CPACK_PACKAGE_DESCRIPTION}" )
	set( CPACK_DEBIAN_PACKAGE_SECTION "non-free/games" )
	set( CPACK_DEBIAN_PACKAGE_PRIORITY "extra" )

	# Fix CPack's wrong filename.
	set( CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}_${CPACK_PACKAGE_VERSION}_${CPACK_SYSTEM_NAME}" )

	include( CPack )
endif()
