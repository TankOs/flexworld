project( FlexWorldTest )

#set(Boost_USE_STATIC_LIBS ON)
find_package( Boost 1.46 REQUIRED unit_test_framework )
find_package( SFML 2.0 )

set(
	SOURCES
	Test.cpp
)

if( NOT Boost_USE_STATIC_LIBS )
	add_definitions( -DBOOST_TEST_DYN_LINK )
endif()

include_directories( "../lib/include/" )
include_directories( ${Boost_INCLUDE_DIR} )
include_directories( ${SFML_INCLUDE_DIR} )

add_executable( test ${SOURCES} )

target_link_libraries( test flexworld ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} )

add_custom_command(
	TARGET test
	POST_BUILD
	COMMAND test
	WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
)
