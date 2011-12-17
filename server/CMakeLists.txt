cmake_minimum_required( VERSION 2.8 )

project( FlexWorldServer )

find_package( SFML 2.0 REQUIRED NETWORK SYSTEM )

set( INC_DIR "src" )
set( SRC_DIR "src" )

set(
	SOURCES
	${SRC_DIR}/DedicatedServer.cpp
)

include_directories( ${PROJECT_SOURCE_DIR}/../lib/include )
include_directories( ${SFML_INCLUDE_DIR} )

add_executable( server ${SOURCES} )

target_link_libraries( server flexworld ${SFML_NETWORK_LIBRARY} ${SFML_SYSTEM_LIBRARY} )

install(
	TARGETS server
	DESTINATION flexworld
)
