project( FlexWorldTest )

set(
	SOURCES
	Test.cpp
)

include_directories( "../lib/include/" )

add_executable( test ${SOURCES} )

target_link_libraries( test flexworld boost_unit_test_framework boost_test_exec_monitor )
