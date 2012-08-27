project( FlexWorldClientTest )

#set( DATA_DIRECTORY ${PROJECT_SOURCE_DIR}/data )
#configure_file( ${PROJECT_SOURCE_DIR}/Config.hpp.in ${PROJECT_BINARY_DIR}/Config.hpp @ONLY )

set( SRC_DIR "${PROJECT_SOURCE_DIR}/src" )
set( INC_DIR "${PROJECT_SOURCE_DIR}/src" )
set( CLIENT_SRC_DIR "${PROJECT_SOURCE_DIR}/../src" )

set(
	SOURCES
	${CLIENT_SRC_DIR}/BufferObjectGroup.cpp
	${CLIENT_SRC_DIR}/ChunkPreparerReader.cpp
	${CLIENT_SRC_DIR}/PlanetDrawable.cpp
	${CLIENT_SRC_DIR}/ResourceManager.cpp
	${SRC_DIR}/Test.cpp
	${SRC_DIR}/TestChunkPreparerReader.cpp
)

if( NOT Boost_USE_STATIC_LIBS )
	add_definitions( -DBOOST_TEST_DYN_LINK )
endif()

include_directories( ${CLIENT_SRC_DIR} )
include_directories( ${PROJECT_SOURCE_DIR}/../../lib/include/ )
include_directories( ${PROJECT_SOURCE_DIR}/../extlibs/FWMS/include/ )
include_directories( ${Boost_INCLUDE_DIR} )
include_directories( ${SFML_INCLUDE_DIR} )

add_executable( client_test ${SOURCES} )

target_link_libraries( client_test flexworld )
target_link_libraries( client_test fwms )
target_link_libraries( client_test ${SFML_SYSTEM_LIBRARY} )
target_link_libraries( client_test ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} )

if( WINDOWS )
else()
	target_link_libraries( client_test ${CMAKE_THREAD_LIBS_INIT} )
	target_link_libraries( client_test rt )
endif()

add_custom_command(
	TARGET client_test
	POST_BUILD
	COMMAND client_test
	WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
)
