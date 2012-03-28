cmake_minimum_required( VERSION 2.8 )

project( FlexWorldLibrary )

set( INC_DIR "include" )
set( SRC_DIR "src" )

#
# Configure files.
#
configure_file( ${PROJECT_SOURCE_DIR}/${INC_DIR}/FlexWorld/Config.hpp.in ${PROJECT_SOURCE_DIR}/${INC_DIR}/FlexWorld/Config.hpp @ONLY )

set(
	SOURCES
	#${INC_DIR}/FlexWorld/SHA1.hpp
	#${SRC_DIR}/FlexWorld/SHA1.cpp
	${INC_DIR}/FlexWorld/Account.hpp
	${INC_DIR}/FlexWorld/AccountDriver.hpp
	${INC_DIR}/FlexWorld/AccountManager.hpp
	${INC_DIR}/FlexWorld/Chunk.hpp
	${INC_DIR}/FlexWorld/Class.hpp
	${INC_DIR}/FlexWorld/ClassCache.hpp
	${INC_DIR}/FlexWorld/ClassDriver.hpp
	${INC_DIR}/FlexWorld/ClassLoader.hpp
	${INC_DIR}/FlexWorld/Client.hpp
	${INC_DIR}/FlexWorld/Client.inl
	${INC_DIR}/FlexWorld/Config.hpp
	${INC_DIR}/FlexWorld/Cuboid.hpp
	${INC_DIR}/FlexWorld/Cuboid.inl
	${INC_DIR}/FlexWorld/Entity.hpp
	${INC_DIR}/FlexWorld/Face.hpp
	${INC_DIR}/FlexWorld/FlexID.hpp
	${INC_DIR}/FlexWorld/GameMode.hpp
	${INC_DIR}/FlexWorld/GameModeDriver.hpp
	${INC_DIR}/FlexWorld/LockFacility.hpp
	${INC_DIR}/FlexWorld/Log.hpp
	${INC_DIR}/FlexWorld/Log.inl
	${INC_DIR}/FlexWorld/LuaModules/Event.hpp
	${INC_DIR}/FlexWorld/LuaModules/Test.hpp
	${INC_DIR}/FlexWorld/Mesh.hpp
	${INC_DIR}/FlexWorld/Message.hpp
	${INC_DIR}/FlexWorld/Messages/Beam.hpp
	${INC_DIR}/FlexWorld/Messages/Chat.hpp
	${INC_DIR}/FlexWorld/Messages/Chunk.hpp
	${INC_DIR}/FlexWorld/Messages/ChunkUnchanged.hpp
	${INC_DIR}/FlexWorld/Messages/CreateEntity.hpp
	${INC_DIR}/FlexWorld/Messages/LoginOK.hpp
	${INC_DIR}/FlexWorld/Messages/OpenLogin.hpp
	${INC_DIR}/FlexWorld/Messages/Ready.hpp
	${INC_DIR}/FlexWorld/Messages/RequestChunk.hpp
	${INC_DIR}/FlexWorld/Messages/ServerInfo.hpp
	${INC_DIR}/FlexWorld/Model.hpp
	${INC_DIR}/FlexWorld/ModelDriver.hpp
	${INC_DIR}/FlexWorld/NonCopyable.hpp
	${INC_DIR}/FlexWorld/PackageEnumerator.hpp
	${INC_DIR}/FlexWorld/Peer.hpp
	${INC_DIR}/FlexWorld/Planet.hpp
	${INC_DIR}/FlexWorld/PlayerInfo.hpp
	${INC_DIR}/FlexWorld/Protocol.hpp
	${INC_DIR}/FlexWorld/Protocol.inl
	${INC_DIR}/FlexWorld/RefLock.hpp
	${INC_DIR}/FlexWorld/Resource.hpp
	${INC_DIR}/FlexWorld/SaveInfo.hpp
	${INC_DIR}/FlexWorld/SaveInfoDriver.hpp
	${INC_DIR}/FlexWorld/ScriptManager.hpp
	${INC_DIR}/FlexWorld/Server.hpp
	${INC_DIR}/FlexWorld/Server.inl
	${INC_DIR}/FlexWorld/ServerProtocol.hpp
	${INC_DIR}/FlexWorld/SessionHost.hpp
	${INC_DIR}/FlexWorld/TemplateUtils.hpp
	${INC_DIR}/FlexWorld/TemplateUtils.inl
	${INC_DIR}/FlexWorld/Version.hpp
	${INC_DIR}/FlexWorld/World.hpp
	${SRC_DIR}/FlexWorld/Account.cpp
	${SRC_DIR}/FlexWorld/AccountDriver.cpp
	${SRC_DIR}/FlexWorld/AccountManager.cpp
	${SRC_DIR}/FlexWorld/Chunk.cpp
	${SRC_DIR}/FlexWorld/Class.cpp
	${SRC_DIR}/FlexWorld/ClassCache.cpp
	${SRC_DIR}/FlexWorld/ClassDriver.cpp
	${SRC_DIR}/FlexWorld/ClassLoader.cpp
	${SRC_DIR}/FlexWorld/Client.cpp
	${SRC_DIR}/FlexWorld/Config.cpp
	${SRC_DIR}/FlexWorld/Entity.cpp
	${SRC_DIR}/FlexWorld/FlexID.cpp
	${SRC_DIR}/FlexWorld/GameMode.cpp
	${SRC_DIR}/FlexWorld/GameModeDriver.cpp
	${SRC_DIR}/FlexWorld/LockFacility.cpp
	${SRC_DIR}/FlexWorld/Log.cpp
	${SRC_DIR}/FlexWorld/LuaModules/Event.cpp
	${SRC_DIR}/FlexWorld/LuaModules/Test.cpp
	${SRC_DIR}/FlexWorld/Math.cpp
	${SRC_DIR}/FlexWorld/Mesh.cpp
	${SRC_DIR}/FlexWorld/Message.cpp
	${SRC_DIR}/FlexWorld/Messages/Beam.cpp
	${SRC_DIR}/FlexWorld/Messages/Chat.cpp
	${SRC_DIR}/FlexWorld/Messages/Chunk.cpp
	${SRC_DIR}/FlexWorld/Messages/ChunkUnchanged.cpp
	${SRC_DIR}/FlexWorld/Messages/CreateEntity.cpp
	${SRC_DIR}/FlexWorld/Messages/LoginOK.cpp
	${SRC_DIR}/FlexWorld/Messages/OpenLogin.cpp
	${SRC_DIR}/FlexWorld/Messages/Ready.cpp
	${SRC_DIR}/FlexWorld/Messages/RequestChunk.cpp
	${SRC_DIR}/FlexWorld/Messages/ServerInfo.cpp
	${SRC_DIR}/FlexWorld/Model.cpp
	${SRC_DIR}/FlexWorld/ModelDriver.cpp
	${SRC_DIR}/FlexWorld/NonCopyable.cpp
	${SRC_DIR}/FlexWorld/PackageEnumerator.cpp
	${SRC_DIR}/FlexWorld/Peer.cpp
	${SRC_DIR}/FlexWorld/Planet.cpp
	${SRC_DIR}/FlexWorld/PlayerInfo.cpp
	${SRC_DIR}/FlexWorld/RefLock.cpp
	${SRC_DIR}/FlexWorld/Resource.cpp
	${SRC_DIR}/FlexWorld/SaveInfo.cpp
	${SRC_DIR}/FlexWorld/SaveInfoDriver.cpp
	${SRC_DIR}/FlexWorld/ScriptManager.cpp
	${SRC_DIR}/FlexWorld/Server.cpp
	${SRC_DIR}/FlexWorld/SessionHost.cpp
	${SRC_DIR}/FlexWorld/Version.cpp
	${SRC_DIR}/FlexWorld/World.cpp
)

include_directories( ${PROJECT_SOURCE_DIR}/include )
include_directories( ${PROJECT_SOURCE_DIR}/../extlibs/FWSG/include/ )
include_directories( ${PROJECT_SOURCE_DIR}/../extlibs/Diluculum/include/ )
include_directories( ${LUA_INCLUDE_DIR} )
include_directories( ${YAML_CPP_INCLUDE_DIR} )
include_directories( ${SFML_INCLUDE_DIR} )
include_directories( ${Boost_INCLUDE_DIR} )

add_library( flexworld ${SOURCES} )

target_link_libraries( flexworld fwsg )
target_link_libraries( flexworld Diluculum )
target_link_libraries( flexworld ${LUA_LIBRARIES} )
target_link_libraries( flexworld ${SFML_SYSTEM_LIBRARY} )
target_link_libraries( flexworld ${YAML_CPP_LIBRARY} )
target_link_libraries( flexworld ${Boost_THREAD_LIBRARY} )
target_link_libraries( flexworld ${Boost_FILESYSTEM_LIBRARY} )
target_link_libraries( flexworld ${Boost_SYSTEM_LIBRARY} )

set_target_properties( flexworld PROPERTIES DEBUG_POSTFIX -s-d )
set_target_properties( flexworld PROPERTIES RELEASE_POSTFIX -s )
