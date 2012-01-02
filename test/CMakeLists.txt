project( FlexWorldTest )

#set(Boost_USE_STATIC_LIBS ON)
find_package( Boost 1.46 REQUIRED unit_test_framework )
find_package( SFML 2.0 )

set( DATA_DIRECTORY ${PROJECT_SOURCE_DIR}/data )
configure_file( ${PROJECT_SOURCE_DIR}/Config.hpp.in ${PROJECT_BINARY_DIR}/Config.hpp @ONLY )

set(
	SOURCES
	${PROJECT_BINARY_DIR}/Config.hpp
	Test.cpp
	Test.cpp
	TestAccount.cpp
	TestAccountDriver.cpp
	TestChunk.cpp
	TestClass.cpp
	TestClassCache.cpp
	TestClassDriver.cpp
	TestFlexID.cpp
	TestMessage.cpp
	TestMessageMeta.cpp
	TestPlanet.cpp
	TestResource.cpp
)

if( NOT Boost_USE_STATIC_LIBS )
	add_definitions( -DBOOST_TEST_DYN_LINK )
endif()

include_directories( "../lib/include/" )
include_directories( ${Boost_INCLUDE_DIR} )
include_directories( ${SFML_INCLUDE_DIR} )
include_directories( ${PROJECT_BINARY_DIR} )

add_executable( test ${SOURCES} )

target_link_libraries( test flexworld ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} )

add_custom_command(
	TARGET test
	POST_BUILD
	COMMAND test
	WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
)
