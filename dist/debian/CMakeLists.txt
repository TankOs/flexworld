cmake_minimum_required( VERSION 2.8 )
project( FlexWorldDeb )

if( FW_PORTABLE_INSTALL OR NOT LINUX )
	message( FATAL_ERROR "Debian package can only be built on Linux and without portable install." )
endif()

# Build final version string.
set( DEB_VERSION "${FW_VERSION_MAJOR}.${FW_VERSION_MINOR}.${FW_VERSION_REVISION}" )

if( FW_VERSION_SUFFIX )
	set( DEB_VERSION "${DEB_VERSION}-${FW_VERSION_SUFFIX}" )
endif()

configure_file(
	${PROJECT_SOURCE_DIR}/control.in
	${PROJECT_SOURCE_DIR}/control
	@ONLY
)

install(
	FILES control
	DESTINATION ../DEBIAN
)

install(
	FILES flexworld.desktop
	DESTINATION share/applications
)

if( CMAKE_SIZEOF_VOID_P MATCHES "8" )
	set( ARCH "amd64" )
else()
	set( ARCH "i386" )
endif()

set( DEB_FILENAME "flexworld_${DEB_VERSION}_${ARCH}.deb" )

install( CODE "execute_process( COMMAND dpkg -b ${CMAKE_INSTALL_PREFIX}/.. ${PROJECT_BINARY_DIR}/${DEB_FILENAME} )" )
install( CODE "message( \"Debian package saved to ${PROJECT_BINARY_DIR}/${DEB_FILENAME}.\" )" )
