cmake_minimum_required( VERSION 2.8 )
project( Diluculum )

# Include directories
include_directories(
	${Boost_INCLUDE_DIRS}
	${LUA_INCLUDE_DIR}
	${PROJECT_SOURCE_DIR}/include
)

# Link directories
link_directories( ${Boost_LIBRARY_DIRS} )

# Build the library
set(
	SOURCES
	Sources/InternalUtils.cpp
	Sources/LuaExceptions.cpp
	Sources/LuaFunction.cpp
	Sources/LuaState.cpp
	Sources/LuaUserData.cpp
	Sources/LuaUtils.cpp
	Sources/LuaValue.cpp
	Sources/LuaVariable.cpp
	Sources/LuaWrappers.cpp
)

add_library( Diluculum STATIC ${SOURCES} )

target_link_libraries( Diluculum ${LUA_LIBRARIES} )

if( LINUX )
	target_link_libraries( Diluculum dl )
endif()
