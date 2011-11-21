cmake_minimum_required( VERSION 2.8 )

project( FlexWorldLibrary )

set( INC_DIR "include" )
set( SRC_DIR "src" )

set(
	SOURCES
	${INC_DIR}/FlexWorld/Class.hpp
	${INC_DIR}/FlexWorld/ClassDriver.hpp
	${INC_DIR}/FlexWorld/Config.hpp
	${INC_DIR}/FlexWorld/Texture.hpp
	${SRC_DIR}/FlexWorld/Class.cpp
	${SRC_DIR}/FlexWorld/ClassDriver.cpp
	${SRC_DIR}/FlexWorld/Config.cpp
	${SRC_DIR}/FlexWorld/Texture.cpp
)

include_directories( ${PROJECT_SOURCE_DIR}/include )
include_directories( ${PROJECT_SOURCE_DIR}/../extlibs/yaml-cpp-0.2.7/include )

add_library( fwlib ${SOURCES} )

target_link_libraries( fwlib yaml-cpp )

set_target_properties( fwlib PROPERTIES DEBUG_POSTFIX -s-d )
set_target_properties( fwlib PROPERTIES RELEASE_POSTFIX -s )
