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
	YAML_CPP_LIBRARY_RELEASE
	NAMES libyaml-cppmd libyaml-cpp libyaml-cpp.a yaml-cpp
	PATH_SUFFIXES lib lib64
	PATHS
		/usr/
		/usr/local/
		${YAML_CPP_DIR}
		$ENV{YAML_CPP_DIR}
)

find_library(
	YAML_CPP_LIBRARY_DEBUG
	NAMES libyaml-cppmdd libyaml-cppd libyaml-cppd.a yaml-cppd
	PATH_SUFFIXES lib lib64
	PATHS
		/usr/
		/usr/local/
		${YAML_CPP_DIR}
		$ENV{YAML_CPP_DIR}
)

if( YAML_CPP_LIBRARY_RELEASE AND YAML_CPP_LIBRARY_DEBUG )
	set( YAML_CPP_LIBRARY debug ${YAML_CPP_LIBRARY_DEBUG} optimized ${YAML_CPP_LIBRARY_RELEASE} )
endif()

if( YAML_CPP_LIBRARY_RELEASE AND NOT YAML_CPP_LIBRARY_DEBUG )
	set( YAML_CPP_LIBRARY_DEBUG ${YAML_CPP_LIBRARY_RELEASE} )
	set( YAML_CPP_LIBRARY ${YAML_CPP_LIBRARY_RELEASE} )
endif()

if( YAML_CPP_LIBRARY_DEBUG AND NOT YAML_CPP_LIBRARY_RELEASE )
	set( YAML_CPP_LIBRARY_RELEASE ${YAML_CPP_LIBRARY_DEBUG} )
	set( YAML_CPP_LIBRARY ${YAML_CPP_LIBRARY_DEBUG} )
endif()

if( NOT YAML_CPP_LIBRARY )
	message( FATAL_ERROR "yaml-cpp library not found. Try setting YAML_CPP_DIR.")
endif()


message( "yaml-cpp headers found: ${YAML_CPP_INCLUDE_DIR}" )
message( "yaml-cpp library found: ${YAML_CPP_LIBRARY}" )
