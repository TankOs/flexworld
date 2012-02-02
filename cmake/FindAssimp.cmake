set( ASSIMP_FOUND false )

find_path(
	ASSIMP_INCLUDE_DIR
	assimp/assimp.h
	PATH_SUFFIXES include
	PATHS
		/usr/
		/usr/local/
		${ASSIMPDIR}
)

find_library(
	ASSIMP_LIBRARY
	NAMES assimp
	PATH_SUFFIXES lib lib64 code
	PATHS
		/usr/
		/usr/local/
		${ASSIMPDIR}
)

if( ASSIMP_INCLUDE_DIR AND ASSIMP_LIBRARY )
	set( ASSIMP_FOUND true )
endif()

if( NOT ASSIMP_FOUND )
	message( FATAL_ERROR "Assimp not found. Try to set ASSIMPDIR." )
else()
	message( "Assimp found: ${ASSIMP_INCLUDE_DIR}" )
endif()
