cmake_minimum_required( VERSION 2.8 )

project( FlexWorldClient )

set(Boost_USE_STATIC_LIBS ON)

find_package( Boost 1.46 REQUIRED filesystem system )
find_package( OpenGL REQUIRED )
find_package( SFGUI REQUIRED )
find_package( SFML 2.0 REQUIRED GRAPHICS WINDOW AUDIO SYSTEM )
find_package( yaml-cpp )

if( NOT WINDOWS )
	find_package( X11 REQUIRED )
endif()

set( INC_DIR "src" )
set( SRC_DIR "src" )

set(
	SOURCES
	${INC_DIR}/Client.hpp
	${INC_DIR}/ColorMorpher.cpp
	${INC_DIR}/ConnectState.hpp
	${INC_DIR}/Controls.hpp
	${INC_DIR}/IntroState.hpp
	${INC_DIR}/KeyNames.hpp
	${INC_DIR}/MenuState.hpp
	${INC_DIR}/OptionsWindow.hpp
	${INC_DIR}/Shared.cpp
	${INC_DIR}/StartGameWindow.hpp
	${INC_DIR}/State.hpp
	${INC_DIR}/UserSettings.cpp
	${SRC_DIR}/Client.cpp
	${SRC_DIR}/ColorMorpher.cpp
	${SRC_DIR}/ConnectState.cpp
	${SRC_DIR}/Controls.cpp
	${SRC_DIR}/IntroState.cpp
	${SRC_DIR}/KeyNames.cpp
	${SRC_DIR}/Main.cpp
	${SRC_DIR}/MenuState.cpp
	${SRC_DIR}/OptionsWindow.cpp
	${SRC_DIR}/Shared.hpp
	${SRC_DIR}/StartGameWindow.cpp
	${SRC_DIR}/State.cpp
	${SRC_DIR}/UserSettings.hpp
)

include_directories( ${PROJECT_SOURCE_DIR}/../lib/include )
include_directories( ${SFGUI_INCLUDE_DIR} )
include_directories( ${OPENGL_INCLUDE_DIR} )
include_directories( ${SFML_INCLUDE_DIR} )
include_directories( ${Boost_INCLUDE_DIR} )
include_directories( ${YAML_CPP_INCLUDE_DIR} )

if( NOT WINDOWS )
	include_directories( ${X11_Xrandr_INCLUDE_PATH} )
	include_directories( ${X11_X11_INCLUDE_PATH} )
endif()

add_executable( flexworld-client ${SOURCES} )

target_link_libraries( flexworld-client flexworld )
target_link_libraries( flexworld-client ${SFGUI_LIBRARY} )
target_link_libraries( flexworld-client ${SFML_GRAPHICS_LIBRARY} ${SFML_WINDOW_LIBRARY} ${SFML_AUDIO_LIBRARY} ${SFML_SYSTEM_LIBRARY} )
target_link_libraries( flexworld-client ${OPENGL_glu_LIBRARY} ${OPENGL_gl_LIBRARY} )
target_link_libraries( flexworld-client ${Boost_FILESYSTEM_LIBRARY} )
target_link_libraries( flexworld-client ${Boost_SYSTEM_LIBRARY} )
target_link_libraries( flexworld-client ${YAML_CPP_LIBRARY} )

if( NOT WINDOWS )
	target_link_libraries( flexworld-client ${X11_Xrandr_LIB} )
endif()

if( FW_PORTABLE_INSTALL OR WINDOWS )
	install(
		TARGETS flexworld-client
		DESTINATION .
	)

	install(
		DIRECTORY local
		DESTINATION .
	)
elseif( LINUX )
	install(
		TARGETS flexworld-client
		DESTINATION bin
	)

	install(
		DIRECTORY local
		DESTINATION games/flexworld
	)
else()
	message( FATAL_ERROR "Install?!?!?!" )
endif()
