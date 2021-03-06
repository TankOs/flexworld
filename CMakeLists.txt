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
set( CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${PROJECT_SOURCE_DIR}/cmake" )
set( CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${PROJECT_SOURCE_DIR}/extlibs/SFML/cmake/Modules" )
set( CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${PROJECT_SOURCE_DIR}/extlibs/FWU/cmake" )
set( CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${PROJECT_SOURCE_DIR}/extlibs/FWCS/cmake" )
set( CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${PROJECT_SOURCE_DIR}/extlibs/FWSG/cmake" )
set( CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${PROJECT_SOURCE_DIR}/extlibs/FWMS/cmake" )
set( CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${PROJECT_SOURCE_DIR}/extlibs/Diluculum/cmake" )

#
# Global project settings.
#
set( FW_VERSION_MAJOR "0" CACHE STRING "Version (major)" FORCE )
set( FW_VERSION_MINOR "0" CACHE STRING "Version (minor)" FORCE )
set( FW_VERSION_REVISION "0" CACHE STRING "Version (revision)" FORCE )
set( FW_VERSION_SUFFIX "" CACHE STRING "Version (suffix)" FORCE )
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

#
# Platform-specific.
#
if( WINDOWS )
	set( BIN_EXTENSION ".exe" )
	set( SHARE_TARGET_DIR . )
	add_definitions( -DWINDOWS )
	add_definitions( -DWIN32_LEAN_AND_MEAN )
else()
	set( SHARE_TARGET_DIR share/flexworld )
	add_definitions( -DLINUX )
endif()

#
# Linker options.
#
set( Boost_USE_STATIC_LIBS false CACHE BOOL "Use Boost's static libraries." )
set( LUA_STATIC_LIBRARIES false CACHE BOOL "Use Lua's static libraries." )
set( SFML_STATIC_LIBRARIES false CACHE BOOL "Was SFML built as a static library?" )

#
# Compiler-specific.
#
if( COMPILER_GCC )
	if( NOT CMAKE_CXX_FLAGS )
		if( NOT MINGW )
			set( CMAKE_CXX_FLAGS "--std=c++11 -Wall -Wextra -Wshadow -Wconversion -pedantic" CACHE STRING "C++ compiler flags" FORCE )
			set( CMAKE_C_FLAGS "--std=c++11 -Wall -Wextra -Wshadow -Wconversion -pedantic" CACHE STRING "C compiler flags" FORCE )
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

find_package( FWU REQUIRED )
find_package( FWCS REQUIRED )
find_package( FWMS REQUIRED )
find_package( FWSG REQUIRED )

if( FW_BUILD_CLIENT )
	set( BOOST_COMPONENTS ${BOOST_COMPONENTS} date_time )
	set( SFML_COMPONENTS ${SFML_COMPONENTS} GRAPHICS AUDIO WINDOW )
	set( LIBROCKET_STATIC_LIBRARIES false CACHE BOOL "Use libRocket's static libraries." )
	find_package( libRocket REQUIRED )
endif()

if( FW_BUILD_TEST )
	set( BOOST_COMPONENTS ${BOOST_COMPONENTS} unit_test_framework )
endif()

find_package( Boost 1.47 REQUIRED COMPONENTS ${BOOST_COMPONENTS} )
find_package( Diluculum REQUIRED )
find_package( GLEW REQUIRED )
find_package( Lua51 REQUIRED )
find_package( OpenAL REQUIRED )
find_package( OpenGL REQUIRED )
find_package( SFML 2.0 REQUIRED COMPONENTS ${SFML_COMPONENTS} )
find_package( Sndfile REQUIRED )
find_package( Threads REQUIRED )
find_package( libnoise REQUIRED )
find_package( yaml-cpp REQUIRED )

if( FW_BUILD_CLIENT )
	if( NOT WINDOWS )
		find_package( X11 REQUIRED )
	endif()
endif()


#
# BUILD.
#

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
