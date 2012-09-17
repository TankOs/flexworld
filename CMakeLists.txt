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

#
# Override CMake module path to use custom/modified Find scripts.
#
set( CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake;${PROJECT_SOURCE_DIR}/extlibs/SFML/cmake/Modules;${CMAKE_MODULE_PATH}" )

#
# Global project settings.
#
set( FW_VERSION_MAJOR "0" CACHE STRING "Version (major)" FORCE )
set( FW_VERSION_MINOR "0" CACHE STRING "Version (minor)" FORCE )
set( FW_VERSION_REVISION "0" CACHE STRING "Version (revision)" FORCE )
set( FW_VERSION_SUFFIX "12-26-0" CACHE STRING "Version (suffix)" FORCE )

set( FW_PORTABLE_INSTALL TRUE CACHE BOOL "Install portable (especially useful for distributing Linux test releases(!), so that both client and server don't use absolute system paths)." )

# Configure data root directory.
if( FW_PORTABLE_INSTALL OR WINDOWS )
	set( FW_INSTALL_ROOT_DATA_DIRECTORY "./" )
	set( GEN_ROOT_DATA_DIRECTORY "${FW_INSTALL_ROOT_DATA_DIRECTORY}" )
elseif( LINUX )
	set( FW_INSTALL_ROOT_DATA_DIRECTORY "share/games/flexworld/" )
	set( GEN_ROOT_DATA_DIRECTORY "${CMAKE_INSTALL_PREFIX}/${FW_INSTALL_ROOT_DATA_DIRECTORY}" )
else()
	message( FATAL_ERROR "Unsupported system." )
endif()

if( NOT FW_ROOT_DATA_DIRECTORY )
	set( FW_ROOT_DATA_DIRECTORY "${GEN_ROOT_DATA_DIRECTORY}" )
else()
	message( "****************************** FlexWorld root data directory overriden: ${FW_ROOT_DATA_DIRECTORY}" )
	message( "****************************** FlexWorld root data directory overriden: ${FW_ROOT_DATA_DIRECTORY}" )
	message( "****************************** FlexWorld root data directory overriden: ${FW_ROOT_DATA_DIRECTORY}" )
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
	add_definitions( -DWIN32_LEAN_AND_MEAN )
else()
	set( SHARE_TARGET_DIR share/FlexWorld )
	add_definitions( -DLINUX )
endif()

#
# Linker options.
#
if( WINDOWS )
	set( Boost_USE_STATIC_LIBS true CACHE BOOL "Use Boost's static libraries." )
	set( LUA_STATIC_LIBRARIES false CACHE BOOL "Use Lua's static libraries." )
	set( SFML_STATIC_LIBRARIES true CACHE BOOL "Was SFML built as a static library?" )
else()
	set( Boost_USE_STATIC_LIBS false CACHE BOOL "Use Boost's static libraries." )
	set( LUA_STATIC_LIBRARIES false CACHE BOOL "Use Lua's static libraries." )
	set( SFML_STATIC_LIBRARIES false CACHE BOOL "Was SFML built as a static library?" )
endif()

#
# Compiler-specific.
#
if( COMPILER_GCC )
	if( NOT CMAKE_CXX_FLAGS )
		if( NOT MINGW )
			set( CMAKE_CXX_FLAGS "--std=c++0x -Wall -Wextra -Wshadow -Wconversion -pedantic" CACHE STRING "C++ compiler flags" FORCE )
			set( CMAKE_C_FLAGS "--std=c++0x -Wall -Wextra -Wshadow -Wconversion -pedantic" CACHE STRING "C compiler flags" FORCE )
		else()
			set( CMAKE_CXX_FLAGS "-std=gnu++11 -Wall -Wextra -Wshadow -Wconversion -pedantic" CACHE STRING "C++ compiler flags" FORCE )
			set( CMAKE_C_FLAGS "-std=gnu++11 -Wall -Wextra -Wshadow -Wconversion -pedantic" CACHE STRING "C compiler flags" FORCE )
		endif()
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
	if( MINGW )
		add_definitions( -DBOOST_THREAD_USE_LIB )
	endif()
endif()

#
# Find packages.
#
set( BOOST_COMPONENTS thread filesystem system )
set( SFML_COMPONENTS SYSTEM )

if( FW_BUILD_CLIENT )
	set( BOOST_COMPONENTS ${BOOST_COMPONENTS} date_time )
	set( SFML_COMPONENTS ${SFML_COMPONENTS} GRAPHICS AUDIO WINDOW )
	find_package( libRocket REQUIRED )
endif()

if( FW_BUILD_TEST )
	set( BOOST_COMPONENTS ${BOOST_COMPONENTS} unit_test_framework )
endif()

find_package( Boost 1.47 REQUIRED COMPONENTS ${BOOST_COMPONENTS} )
find_package( SFML 2.0 REQUIRED COMPONENTS ${SFML_COMPONENTS} )
find_package( yaml-cpp REQUIRED )
find_package( Threads REQUIRED )
find_package( Lua51 REQUIRED )
find_package( libnoise REQUIRED )

if( FW_BUILD_CLIENT )
	if( NOT WINDOWS )
		find_package( X11 REQUIRED )
		find_package( OpenAL REQUIRED )
	else()
		find_package( OpenGL REQUIRED )
	endif()
endif()

if( MINGW )
	find_package( GLEW REQUIRED )
	add_definitions( -DGLEW_STATIC )
endif()

#
# BUILD.
#

# FWSG.
if( FW_BUILD_CLIENT )
	set( FWSG_BUILD_EXAMPLES FALSE CACHE BOOL "" )
	set( FWSG_BUILD_TEST FALSE CACHE BOOL "" )
	set( FWSG_SKIP_INSTALL TRUE CACHE BOOL "" )
	add_subdirectory( "extlibs/FWSG" )
endif()

# FWMS.
set( FWMS_SKIP_INSTALL TRUE CACHE BOOL "" )
set( FWMS_BUILD_EXAMPLES FALSE CACHE BOOL "" )
set( FWMS_BUILD_TEST FALSE CACHE BOOL "" )
set( FWMS_BUILD_DOC FALSE CACHE BOOL "" )
set( FWMS_BUILD_SHARED_LIBS FALSE CACHE BOOL "" )
add_subdirectory( "extlibs/FWMS" )

# FWCS.
set( FWCS_SKIP_INSTALL TRUE CACHE BOOL "" )
set( FWCS_BUILD_SHARED_LIBS FALSE CACHE BOOL "" )
set( FWCS_BUILD_TEST FALSE CACHE BOOL "" )
set( FWCS_BUILD_DOCS FALSE CACHE BOOL "" )
set( FWCS_BUILD_EXAMPLES FALSE CACHE BOOL "" )
add_subdirectory( "extlibs/FWCS" )

# Diluculum.
add_subdirectory( "extlibs/Diluculum" )

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
	# Build Assimp.
	set( BUILD_ASSIMP_SAMPLES FALSE CACHE BOOL "" FORCE )
	set( BUILD_ASSIMP_TOOLS FALSE CACHE BOOL "" FORCE )
	set( BUILD_STATIC_LIB TRUE CACHE BOOL "" FORCE )
	add_subdirectory( "extlibs/assimp/" )

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
