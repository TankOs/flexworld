cmake_minimum_required( VERSION 2.8 )

project( FlexWorldServer )

set( INC_DIR "src" )
set( SRC_DIR "src" )

set(
	SOURCES
	${SRC_DIR}/DedicatedServer.cpp
)

include_directories( ${PROJECT_SOURCE_DIR}/../lib/include )
include_directories( ${SFML_INCLUDE_DIR} )

add_executable( flexworld-server ${SOURCES} )

target_link_libraries( flexworld-server flexworld ${SFML_SYSTEM_LIBRARY} )
target_link_libraries( flexworld-server ${FWU_LIBRARY} )

if( FW_PORTABLE_INSTALL OR WINDOWS )
	install(
		TARGETS flexworld-server
		DESTINATION .
	)
elseif( LINUX )
	install(
		TARGETS flexworld-server
		DESTINATION games
	)
else()
	message( FATAL_ERROR "Install?!?!?!" )
endif()
