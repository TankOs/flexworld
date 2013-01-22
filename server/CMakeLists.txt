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
include_directories( ${YAML_CPP_INCLUDE_DIR} )
include_directories( ${Diluculum_INCLUDE_DIR} )

add_executable( flexworld-server ${SOURCES} )

target_link_libraries( flexworld-server flexworld )
target_link_libraries( flexworld-server ${FWSG_LIBRARY} )
target_link_libraries( flexworld-server ${FWMS_LIBRARY} )
target_link_libraries( flexworld-server ${FWCS_LIBRARY} )
target_link_libraries( flexworld-server ${FWU_LIBRARY} )
target_link_libraries( flexworld-server ${Diluculum_LIBRARY} )
target_link_libraries( flexworld-server ${LIBNOISE_LIBRARY} )
target_link_libraries( flexworld-server ${LUA_LIBRARIES} )
target_link_libraries( flexworld-server ${YAML_CPP_LIBRARY} )
target_link_libraries( flexworld-server ${SFML_GRAPHICS_LIBRARY} )
target_link_libraries( flexworld-server ${SFML_SYSTEM_LIBRARY} )
target_link_libraries( flexworld-server ${Boost_FILESYSTEM_LIBRARY} )
target_link_libraries( flexworld-server ${Boost_THREAD_LIBRARY} )

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
