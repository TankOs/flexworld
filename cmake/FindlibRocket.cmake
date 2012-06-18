# Input variables:
#   - LIBROCKET_ROOT: Additional search directory (either CMake variable or environment variable).
#
# Output variables:
#   - LIBROCKET_INCLUDE_DIR: Include directory.
#   - LIBROCKET_CORE_LIBRARY: libRocket's core library.
#   - LIBROCKET_CONTROLS_LIBRARY: libRocket's controls library.
#   - LIBROCKET_DEBUGGER_LIBRARY: libRocket's debugger library.
#   - LIBROCKET_LIBRARIES: All libraries.

set( LIBROCKET_FOUND false )
set( LIBROCKET_STATIC_LIBRARIES TRUE CACHE BOOL "Use static libRocket libraries." )

if( LIBROCKET_STATIC_LIBRARIES )
	add_definitions( -DSTATIC_LIB )
endif()

find_path(
	LIBROCKET_INCLUDE_DIR
	Rocket/Core.h
	PATH_SUFFIXES include Include
	PATHS
		/usr/
		/usr/local/
		${LIBROCKET_ROOT}
		$ENV{LIBROCKET_ROOT}
)

if( NOT LIBROCKET_INCLUDE_DIR )
	message( FATAL_ERROR "libRocket headers not found. Try setting LIBROCKET_ROOT.")
endif()

find_library(
	LIBROCKET_CORE_LIBRARY
	NAMES libRocketCore RocketCore
	PATH_SUFFIXES lib lib64
	PATHS
		/usr/
		/usr/local/
		${LIBROCKET_ROOT}
		$ENV{LIBROCKET_ROOT}
)

find_library(
	LIBROCKET_CONTROLS_LIBRARY
	NAMES libRocketControls RocketControls
	PATH_SUFFIXES lib lib64
	PATHS
		/usr/
		/usr/local/
		${LIBROCKET_ROOT}
		$ENV{LIBROCKET_ROOT}
)

find_library(
	LIBROCKET_DEBUGGER_LIBRARY
	NAMES libRocketDebugger RocketDebugger
	PATH_SUFFIXES lib lib64
	PATHS
		/usr/
		/usr/local/
		${LIBROCKET_ROOT}
		$ENV{LIBROCKET_ROOT}
)

if( NOT LIBROCKET_CORE_LIBRARY )
	message( FATAL_ERROR "libRocket core library not found. Try setting LIBROCKET_ROOT.")
endif()

if( NOT LIBROCKET_CONTROLS_LIBRARY )
	message( FATAL_ERROR "libRocket controls library not found. Try setting LIBROCKET_ROOT.")
endif()

if( NOT LIBROCKET_DEBUGGER_LIBRARY )
	message( FATAL_ERROR "libRocket debugger library not found. Try setting LIBROCKET_ROOT.")
endif()

set( LIBROCKET_LIBRARIES "${LIBROCKET_CORE_LIBRARY};${LIBROCKET_CONTROLS_LIBRARY};${LIBROCKET_DEBUGGER_LIBRARY}" )

message( "libRocket headers found: ${LIBROCKET_INCLUDE_DIR}" )
message( "libRocket libraries found: ${LIBROCKET_LIBRARIES}" )
