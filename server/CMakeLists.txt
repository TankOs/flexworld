cmake_minimum_required( VERSION 2.8 )

project( FlexWorldServer )

set( INC_DIR "src" )
set( SRC_DIR "src" )

set(
	SOURCES
	${SRC_DIR}/DedicatedServer.cpp
)

include_directories( ${PROJECT_SOURCE_DIR}/../include )

add_executable( fwserver ${SOURCES} )

target_link_libraries( fwserver flexworld ${SFML_NETWORK_LIBRARY} ${SFML_SYSTEM_LIBRARY} )
