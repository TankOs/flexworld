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

if( NOT YAML_CPP_INCLUDE_DIR )
	message( FATAL_ERROR "yaml-cpp headers not found. Try setting YAML_CPP_DIR.")
endif()

find_library(
	YAML_CPP_LIBRARY
	NAMES libyaml-cppmd libyaml-cpp libyaml-cpp.a
	PATH_SUFFIXES lib lib64
	PATHS
		/usr/
		/usr/local/
		${YAML_CPP_DIR}
		$ENV{YAML_CPP_DIR}
)

if( NOT YAML_CPP_LIBRARY )
	message( FATAL_ERROR "yaml-cpp library not found. Try setting YAML_CPP_DIR.")
endif()


message( "yaml-cpp headers found: ${YAML_CPP_INCLUDE_DIR}" )
message( "yaml-cpp library found: ${YAML_CPP_LIBRARY}" )
