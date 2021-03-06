cmake_minimum_required( VERSION 2.8 )

project( FlexWorldClient )

set( INC_DIR "src" )
set( SRC_DIR "src" )

set(
	SOURCES
	${INC_DIR}/BufferObjectGroup.hpp
	${INC_DIR}/Camera.hpp
	${INC_DIR}/CameraReader.hpp
	${INC_DIR}/ChatController.hpp
	${INC_DIR}/ClassDrawable.hpp
	${INC_DIR}/Client.hpp
	${INC_DIR}/ColorMorpher.cpp
	${INC_DIR}/ColorPicker.hpp
	${INC_DIR}/ComponentSystemReader.hpp
	${INC_DIR}/ConnectState.hpp
	${INC_DIR}/ContainerController.hpp
	${INC_DIR}/ContainerManager.hpp
	${INC_DIR}/Controls.hpp
	${INC_DIR}/DebugReader.hpp
	${INC_DIR}/Desk.hpp
	${INC_DIR}/EntityGroupNode.hpp
	${INC_DIR}/HostSyncReader.hpp
	${INC_DIR}/IntroState.hpp
	${INC_DIR}/KeyNames.hpp
	${INC_DIR}/MenuState.hpp
	${INC_DIR}/MessageHandler.hpp
	${INC_DIR}/MovementReader.hpp
	${INC_DIR}/OptionsDocumentController.hpp
	${INC_DIR}/PlanetDrawable.hpp
	${INC_DIR}/PlayState.hpp
	${INC_DIR}/ResourceManager.hpp
	${INC_DIR}/RocketEventDispatcher.hpp
	${INC_DIR}/RocketRenderInterface.hpp
	${INC_DIR}/RocketSystemInterface.hpp
	${INC_DIR}/RocketUtils.hpp
	${INC_DIR}/SceneGraphReader.hpp
	${INC_DIR}/SessionState.hpp
	${INC_DIR}/SessionStateReader.hpp
	${INC_DIR}/Shared.cpp
	${INC_DIR}/State.hpp
	${INC_DIR}/StateFactory.hpp
	${INC_DIR}/TextScroller.hpp
	${INC_DIR}/TextureFilter.hpp
	${INC_DIR}/UserInterface.hpp
	${INC_DIR}/UserSettings.cpp
	${INC_DIR}/WorldSyncReader.hpp
	${SRC_DIR}/BufferObjectGroup.cpp
	${SRC_DIR}/Camera.cpp
	${SRC_DIR}/CameraReader.cpp
	${SRC_DIR}/ChatController.cpp
	${SRC_DIR}/ClassDrawable.cpp
	${SRC_DIR}/Client.cpp
	${SRC_DIR}/ColorMorpher.cpp
	${SRC_DIR}/ColorPicker.cpp
	${SRC_DIR}/ComponentSystemReader.cpp
	${SRC_DIR}/ConnectState.cpp
	${SRC_DIR}/ContainerController.cpp
	${SRC_DIR}/ContainerManager.cpp
	${SRC_DIR}/Controls.cpp
	${SRC_DIR}/DebugReader.cpp
	${SRC_DIR}/Desk.cpp
	${SRC_DIR}/EntityGroupNode.cpp
	${SRC_DIR}/HostSyncReader.cpp
	${SRC_DIR}/IntroState.cpp
	${SRC_DIR}/KeyNames.cpp
	${SRC_DIR}/Main.cpp
	${SRC_DIR}/MenuState.cpp
	${SRC_DIR}/MessageHandler.cpp
	${SRC_DIR}/MovementReader.cpp
	${SRC_DIR}/OptionsDocumentController.cpp
	${SRC_DIR}/PlanetDrawable.cpp
	${SRC_DIR}/PlayState.cpp
	${SRC_DIR}/ResourceManager.cpp
	${SRC_DIR}/RocketEventDispatcher.cpp
	${SRC_DIR}/RocketRenderInterface.cpp
	${SRC_DIR}/RocketSystemInterface.cpp
	${SRC_DIR}/RocketUtils.cpp
	${SRC_DIR}/SceneGraphReader.cpp
	${SRC_DIR}/SessionState.cpp
	${SRC_DIR}/SessionStateReader.cpp
	${SRC_DIR}/Shared.hpp
	${SRC_DIR}/State.cpp
	${SRC_DIR}/StateFactory.cpp
	${SRC_DIR}/TextScroller.cpp
	${SRC_DIR}/UserInterface.cpp
	${SRC_DIR}/UserSettings.hpp
	${SRC_DIR}/WorldSyncReader.cpp
)

include_directories( ${PROJECT_SOURCE_DIR}/../lib/include )
include_directories( ${FWU_INCLUDE_DIR} )
include_directories( ${FWSG_INCLUDE_DIR} )
include_directories( ${FWMS_INCLUDE_DIR} )
include_directories( ${FWCS_INCLUDE_DIR} )
include_directories( ${PROJECT_SOURCE_DIR}/../extlibs/SFML/extlibs/headers/ )
include_directories( ${LIBROCKET_INCLUDE_DIR} )
include_directories( ${OPENGL_INCLUDE_DIR} )
include_directories( ${SFML_INCLUDE_DIR} )
include_directories( ${Boost_INCLUDE_DIR} )
include_directories( ${YAML_CPP_INCLUDE_DIR} )
include_directories( ${Diluculum_INCLUDE_DIR} )

if( NOT WINDOWS )
	include_directories( ${X11_Xrandr_INCLUDE_PATH} )
	include_directories( ${X11_X11_INCLUDE_PATH} )
	include_directories( ${OPENAL_INCLUDE_DIR} )
	include_directories( ${GLEW_INCLUDE_PATH} )
endif()

add_executable( flexworld-client ${SOURCES} )

target_link_libraries( flexworld-client flexworld )

target_link_libraries( flexworld-client ${FWSG_LIBRARY} )
target_link_libraries( flexworld-client ${FWMS_LIBRARY} )
target_link_libraries( flexworld-client ${FWCS_LIBRARY} )
target_link_libraries( flexworld-client ${FWU_LIBRARY} )
target_link_libraries( flexworld-client ${Diluculum_LIBRARY} )

target_link_libraries( flexworld-client ${LIBROCKET_LIBRARIES} )

target_link_libraries( flexworld-client ${SFML_GRAPHICS_LIBRARY} )
target_link_libraries( flexworld-client ${SFML_WINDOW_LIBRARY} )
target_link_libraries( flexworld-client ${SFML_AUDIO_LIBRARY} )
target_link_libraries( flexworld-client ${SFML_SYSTEM_LIBRARY} )

target_link_libraries( flexworld-client ${YAML_CPP_LIBRARY} )

target_link_libraries( flexworld-client ${GLEW_LIBRARY} )
target_link_libraries( flexworld-client ${OPENGL_LIBRARIES} )

target_link_libraries( flexworld-client ${SNDFILE_LIBRARIES} )
target_link_libraries( flexworld-client ${OPENAL_LIBRARY} )

target_link_libraries( flexworld-client ${LIBNOISE_LIBRARY} )
target_link_libraries( flexworld-client ${LUA_LIBRARIES} )

target_link_libraries( flexworld-client ${Boost_THREAD_LIBRARY} )
target_link_libraries( flexworld-client ${Boost_FILESYSTEM_LIBRARY} )
target_link_libraries( flexworld-client ${Boost_SYSTEM_LIBRARY} )

if( NOT WINDOWS )
	target_link_libraries( flexworld-client ${X11_Xrandr_LIB} )
	target_link_libraries( flexworld-client jpeg )
	target_link_libraries( flexworld-client freetype )
	target_link_libraries( flexworld-client ${CMAKE_THREAD_LIBS_INIT} )
	target_link_libraries( flexworld-client rt )
elseif( WINDOWS AND MINGW )
	target_link_libraries( flexworld-client ws2_32 )
	target_link_libraries( flexworld-client mswsock )
endif()

# Binaries.
if( FW_PORTABLE_INSTALL OR WINDOWS )
	install(
		TARGETS flexworld-client
		DESTINATION .
	)
elseif( LINUX )
	install(
		TARGETS flexworld-client
		DESTINATION games
	)
else()
	message( FATAL_ERROR "Install?!?!?!" )
endif()

# Data.
install(
	DIRECTORY local
	DESTINATION "${FW_INSTALL_ROOT_DATA_DIRECTORY}"
)
