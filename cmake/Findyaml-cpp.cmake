set( YAML_CPP_FOUND false )

find_path(
	YAML_CPP_INCLUDE_DIR
	yaml-cpp/yaml.h
	PATH_SUFFIXES include
	PATHS
		/usr/
		/usr/local/
		${YAML_CPP_DIR}
		$ENV{YAML_CPP_DIR}
)

find_library(
	YAML_CPP_LIBRARY
	NAMES libyaml-cppmd
	PATH_SUFFIXES lib lib64
	PATHS
		/usr/
		/usr/local/
		${YAML_CPP_DIR}
		$ENV{YAML_CPP_DIR}
)

if( YAML_CPP_INCLUDE_DIR AND YAML_CPP_LIBRARY )
	set( YAML_CPP_FOUND true )
endif()

if( NOT YAML_CPP_FOUND )
	message( FATAL_ERROR "yaml-cpp not found. Try setting YAML_CPP_DIR" )
else()
	message( "yaml-cpp headers found: ${YAML_CPP_INCLUDE_DIR}" )
	message( "yaml-cpp lib found: ${YAML_CPP_LIBRARY}" )
endif()
