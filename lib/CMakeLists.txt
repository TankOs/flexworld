cmake_minimum_required( VERSION 2.8 )

project( FlexWorldLibrary )

set( INC_DIR "include" )
set( SRC_DIR "src" )

set(
	SOURCES
	${INC_DIR}/FlexWorld/Class.hpp
	${INC_DIR}/FlexWorld/ClassDriver.hpp
	${INC_DIR}/FlexWorld/Config.hpp
	${INC_DIR}/FlexWorld/Resource.hpp
	${INC_DIR}/FlexWorld/ResourceId.hpp
	${INC_DIR}/FlexWorld/Server.hpp
	${SRC_DIR}/FlexWorld/Class.cpp
	${SRC_DIR}/FlexWorld/ClassDriver.cpp
	${SRC_DIR}/FlexWorld/Config.cpp
	${SRC_DIR}/FlexWorld/Resource.cpp
	${SRC_DIR}/FlexWorld/ResourceId.cpp
	${SRC_DIR}/FlexWorld/Server.cpp
)

include_directories( ${PROJECT_SOURCE_DIR}/include )
include_directories( ${PROJECT_SOURCE_DIR}/../extlibs/yaml-cpp-0.2.7/include )

add_library( flexworld ${SOURCES} )

target_link_libraries( flexworld yaml-cpp )

set_target_properties( flexworld PROPERTIES DEBUG_POSTFIX -s-d )
set_target_properties( flexworld PROPERTIES RELEASE_POSTFIX -s )
