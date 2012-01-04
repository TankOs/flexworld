cmake_minimum_required( VERSION 2.8 )
project( FlexWorldGameModes )

function( install_game_mode GM_NAME )
	set( SOURCE_DIR "${PROJECT_SOURCE_DIR}/${GM_NAME}" )

	if( IS_DIRECTORY "${SOURCE_DIR}/modes" )
		INSTALL( DIRECTORY ${GM_NAME}/modes DESTINATION ${FW_ROOT_DATA_DIRECTORY} )
	endif()

	if( IS_DIRECTORY "${SOURCE_DIR}/packages" )
		INSTALL( DIRECTORY ${GM_NAME}/packages DESTINATION ${FW_ROOT_DATA_DIRECTORY} )
	endif()
endfunction()

set( FW_INSTALL_GM_TUTORIAL TRUE CACHE BOOL "Install tutorial game mode." )
set( FW_INSTALL_GM_SANDBOX TRUE CACHE BOOL "Install sandbox game mode." )

# Install base stuff.
install_game_mode( "base" )

if( FW_INSTALL_GM_TUTORIAL )
	install_game_mode( "tutorial" )
endif()

if( FW_INSTALL_GM_SANDBOX )
	install_game_mode( "sandbox" )
endif()
