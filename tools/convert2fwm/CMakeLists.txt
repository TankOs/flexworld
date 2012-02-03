cmake_minimum_required( VERSION 2.6 )
project( convert2fwm )

find_package( Assimp REQUIRED )

set( INC_ROOT ${PROJECT_SOURCE_DIR}/src )
set( SRC_ROOT ${PROJECT_SOURCE_DIR}/src )

set(
	SOURCES
	${SRC_ROOT}/Convert2FWM.cpp
)

include_directories( ${PROJECT_SOURCE_DIR}/../../lib/include/ )
include_directories( ${ASSIMP_INCLUDE_DIR} )

add_executable( convert2fwm ${SOURCES} )
target_link_libraries( convert2fwm flexworld ${ASSIMP_LIBRARY} )

if( FW_PORTABLE_INSTALL OR WINDOWS )
	install(
		TARGETS convert2fwm
		DESTINATION .
	)
elseif( LINUX )
	install(
		TARGETS convert2fwm
		DESTINATION bin
	)
else()
	message( FATAL_ERROR "Install?!?!?!" )
endif()
