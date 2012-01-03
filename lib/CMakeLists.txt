cmake_minimum_required( VERSION 2.8 )

project( FlexWorldLibrary )

find_package( SFML 2.0 REQUIRED NETWORK SYSTEM )
find_package( yaml-cpp )
find_package( Boost 1.46 REQUIRED filesystem system )

set( INC_DIR "include" )
set( SRC_DIR "src" )

set(
	SOURCES
	#${INC_DIR}/FlexWorld/Config.hpp
	#${INC_DIR}/FlexWorld/SHA1.hpp
	#${SRC_DIR}/FlexWorld/Config.cpp
	#${SRC_DIR}/FlexWorld/SHA1.cpp
	${INC_DIR}/FlexWorld/Account.hpp
	${INC_DIR}/FlexWorld/AccountDriver.hpp
	${INC_DIR}/FlexWorld/AccountManager.hpp
	${INC_DIR}/FlexWorld/Chunk.hpp
	${INC_DIR}/FlexWorld/Class.hpp
	${INC_DIR}/FlexWorld/ClassCache.hpp
	${INC_DIR}/FlexWorld/ClassDriver.hpp
	${INC_DIR}/FlexWorld/FlexID.hpp
	${INC_DIR}/FlexWorld/Message.hpp
	${INC_DIR}/FlexWorld/MessageMeta.hpp
	${INC_DIR}/FlexWorld/NonCopyable.hpp
	${INC_DIR}/FlexWorld/Planet.hpp
	${INC_DIR}/FlexWorld/Resource.hpp
	${INC_DIR}/FlexWorld/SaveInfo.hpp
	${INC_DIR}/FlexWorld/SaveInfoDriver.hpp
	${INC_DIR}/FlexWorld/Server.hpp
	${SRC_DIR}/FlexWorld/Account.cpp
	${SRC_DIR}/FlexWorld/AccountDriver.cpp
	${SRC_DIR}/FlexWorld/AccountManager.cpp
	${SRC_DIR}/FlexWorld/Chunk.cpp
	${SRC_DIR}/FlexWorld/Class.cpp
	${SRC_DIR}/FlexWorld/ClassCache.cpp
	${SRC_DIR}/FlexWorld/ClassDriver.cpp
	${SRC_DIR}/FlexWorld/FlexID.cpp
	${SRC_DIR}/FlexWorld/Message.cpp
	${SRC_DIR}/FlexWorld/MessageMeta.cpp
	${SRC_DIR}/FlexWorld/NonCopyable.cpp
	${SRC_DIR}/FlexWorld/Planet.cpp
	${SRC_DIR}/FlexWorld/Resource.cpp
	${SRC_DIR}/FlexWorld/SaveInfo.cpp
	${SRC_DIR}/FlexWorld/SaveInfoDriver.cpp
	${SRC_DIR}/FlexWorld/Server.cpp
)

include_directories( ${PROJECT_SOURCE_DIR}/include )
include_directories( ${YAML_CPP_INCLUDE_DIR} )
include_directories( ${SFML_INCLUDE_DIR} )
include_directories( ${Boost_INCLUDE_DIR} )

add_library( flexworld ${SOURCES} )

target_link_libraries( flexworld ${YAML_CPP_LIBRARY} )
target_link_libraries( flexworld ${Boost_FILESYSTEM_LIBRARY} )
target_link_libraries( flexworld ${Boost_SYSTEM_LIBRARY} )

set_target_properties( flexworld PROPERTIES DEBUG_POSTFIX -s-d )
set_target_properties( flexworld PROPERTIES RELEASE_POSTFIX -s )
