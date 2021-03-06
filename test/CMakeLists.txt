project( FlexWorldTest )

set( DATA_DIRECTORY ${PROJECT_SOURCE_DIR}/data )
configure_file( ${PROJECT_SOURCE_DIR}/Config.hpp.in ${PROJECT_BINARY_DIR}/Config.hpp @ONLY )

set(
	SOURCES
	${PROJECT_BINARY_DIR}/Config.hpp
	ExampleServerGate.cpp
	ExampleServerGate.hpp
	ExampleWorldGate.cpp
	ExampleWorldGate.hpp
	ExceptionChecker.hpp
	LuaUtils.cpp
	LuaUtils.hpp
	Test.cpp
	TestAccount.cpp
	TestAccountDriver.cpp
	TestAccountManager.cpp
	TestChunk.cpp
	TestClass.cpp
	TestClassCache.cpp
	TestClassDriver.cpp
	TestClassLoader.cpp
	TestClient.cpp
	TestEntity.cpp
	TestEntityWatchdogController.cpp
	TestEventLuaModule.cpp
	TestFlexID.cpp
	TestGameMode.cpp
	TestGameModeDriver.cpp
	TestLockFacility.cpp
	TestMesh.cpp
	TestMessage.cpp
	TestModel.cpp
	TestModelDriver.cpp
	TestPackageEnumerator.cpp
	TestPlanet.cpp
	TestRefLock.cpp
	TestResource.cpp
	TestSaveInfo.cpp
	TestSaveInfoDriver.cpp
	TestScriptManager.cpp
	TestServer.cpp
	TestServerLuaModule.cpp
	TestServerProtocol.cpp
	TestSessionHost.cpp
	TestTerrainGenerator.cpp
	TestTestLuaModule.cpp
	TestVersion.cpp
	TestWorld.cpp
	TestWorldLuaModule.cpp
)

if( NOT Boost_USE_STATIC_LIBS )
	add_definitions( -DBOOST_TEST_DYN_LINK )
endif()

include_directories( ${PROJECT_SOURCE_DIR}/../lib/include/ )
include_directories( ${PROJECT_SOURCE_DIR}/../extlibs/FWSG/include/ )
include_directories( ${PROJECT_SOURCE_DIR}/../extlibs/FWCS/include/ )
include_directories( ${PROJECT_SOURCE_DIR}/../extlibs/FWMS/include/ )
include_directories( ${PROJECT_SOURCE_DIR}/../extlibs/Diluculum/include/ )
include_directories( ${LUA_INCLUDE_DIR} )
include_directories( ${Boost_INCLUDE_DIR} )
include_directories( ${SFML_INCLUDE_DIR} )
include_directories( ${PROJECT_BINARY_DIR} )

add_executable( test ${SOURCES} )

target_link_libraries( test flexworld )
target_link_libraries( test ${FWSG_LIBRARY} )
target_link_libraries( test ${FWMS_LIBRARY} )
target_link_libraries( test ${FWCS_LIBRARY} )
target_link_libraries( test ${FWU_LIBRARY} )
target_link_libraries( test ${Diluculum_LIBRARY} )
target_link_libraries( test ${YAML_CPP_LIBRARY} )
target_link_libraries( test ${LUA_LIBRARIES} )
target_link_libraries( test ${SFML_GRAPHICS_LIBRARY} )
target_link_libraries( test ${SFML_SYSTEM_LIBRARY} )
target_link_libraries( test ${Boost_THREAD_LIBRARY} )
target_link_libraries( test ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} )
target_link_libraries( test ${Boost_FILESYSTEM_LIBRARY} )
target_link_libraries( test ${Boost_SYSTEM_LIBRARY} )
target_link_libraries( test ${LIBNOISE_LIBRARY} )

if( WINDOWS )
	target_link_libraries( test ws2_32 )
	target_link_libraries( test mswsock )
else()
	target_link_libraries( test ${CMAKE_THREAD_LIBS_INIT} )
	target_link_libraries( test rt )
endif()

add_custom_command(
	TARGET test
	POST_BUILD
	COMMAND test
	WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
)
