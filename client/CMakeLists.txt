cmake_minimum_required( VERSION 2.8 )

project( FlexWorldClient )

find_package( OpenGL REQUIRED )
find_package( GLEW REQUIRED )
find_package( JPEG REQUIRED )
find_package( Freetype REQUIRED )
find_package( SFGUI REQUIRED )
find_package( SFML 2.0 REQUIRED GRAPHICS WINDOW AUDIO SYSTEM )

if( NOT WINDOWS )
	find_package( X11 REQUIRED )
endif()

set( INC_DIR "src" )
set( SRC_DIR "src" )

set(
	SOURCES
	${INC_DIR}/Client.hpp
	${INC_DIR}/IntroState.hpp
	${INC_DIR}/MenuState.hpp
	${INC_DIR}/OptionsWindow.hpp
	${INC_DIR}/PlayWindow.hpp
	${INC_DIR}/State.hpp
	${SRC_DIR}/Client.cpp
	${SRC_DIR}/IntroState.cpp
	${SRC_DIR}/Main.cpp
	${SRC_DIR}/MenuState.cpp
	${SRC_DIR}/OptionsWindow.cpp
	${SRC_DIR}/PlayWindow.cpp
	${SRC_DIR}/State.cpp
)

include_directories( ${PROJECT_SOURCE_DIR}/../lib/include )
include_directories( ${SFGUI_INCLUDE_DIR} )
include_directories( ${OPENGL_INCLUDE_DIR} )
include_directories( ${GLEW_INCLUDE_PATH} )
include_directories( ${JPEG_INCLUDE_DIR} )
include_directories( ${FREETYPE_INCLUDE_DIR_freetype2} )
include_directories( ${FREETYPE_INCLUDE_DIR_ft2build} )

if( NOT WINDOWS )
	include_directories( ${X11_Xrandr_INCLUDE_PATH} )
	include_directories( ${X11_X11_INCLUDE_PATH} )
endif()

add_executable( client ${SOURCES} )

target_link_libraries( client flexworld )
target_link_libraries( client ${SFGUI_LIBRARY} )
target_link_libraries( client ${SFML_GRAPHICS_LIBRARY} ${SFML_WINDOW_LIBRARY} ${SFML_AUDIO_LIBRARY} ${SFML_SYSTEM_LIBRARY} )
target_link_libraries( client ${GLEW_LIBRARY} )
target_link_libraries( client ${OPENGL_glu_LIBRARY} ${OPENGL_gl_LIBRARY} )
target_link_libraries( client ${JPEG_LIBRARY} )
target_link_libraries( client ${FREETYPE_LIBRARY} )

if( NOT WINDOWS )
	target_link_libraries( client ${X11_Xrandr_LIB} )
endif()

install(
	TARGETS client
	DESTINATION flexworld
)

install(
	DIRECTORY data
	DESTINATION flexworld
)
