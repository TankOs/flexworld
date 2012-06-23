set( LIBNOISE_FOUND false )

find_path(
	LIBNOISE_INCLUDE_DIR
	libnoise/noise.h
	PATH_SUFFIXES include
	PATHS
		/usr/
		/usr/local/
		${LIBNOISE_ROOT}
		$ENV{LIBNOISE_ROOT}
)

if( NOT LIBNOISE_INCLUDE_DIR )
	message( FATAL_ERROR "libnoise headers not found. Try setting LIBNOISE_ROOT.")
endif()

find_library(
	LIBNOISE_LIBRARY
	NAMES libnoise.so libnoise.a libnoise
	PATH_SUFFIXES lib lib64
	PATHS
		/usr/
		/usr/local/
		${LIBNOISE_ROOT}
		$ENV{LIBNOISE_ROOT}
)

if( NOT LIBNOISE_LIBRARY )
	message( FATAL_ERROR "libnoise library not found. Try setting LIBNOISE_ROOT.")
endif()


message( "libnoise headers found: ${LIBNOISE_INCLUDE_DIR}" )
message( "libnoise library found: ${LIBNOISE_LIBRARY}" )
