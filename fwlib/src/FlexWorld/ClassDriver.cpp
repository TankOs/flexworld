#include <FlexWorld/ClassDriver.hpp>
#include <FlexWorld/Class.hpp>

#include <yaml-cpp/yaml.h>
#include <fstream>

namespace flex {

bool ClassDriver::load( const std::string& path, Class& cls ) {
	Class new_class;

	// Open file.
	std::ifstream in( path.c_str() );
	if( !in.is_open() ) {
		return false;
	}

	// Setup parser.
	YAML::Parser parser( in );
	YAML::Node doc;

	while( parser.GetNextDocument( doc ) ) {
		std::string string;

		try {
			doc["Name"] >> string;
			new_class.set_name( string );
		}
		catch( ... ) {
			return false;
		}
	}

	// Everything okay.
	cls = new_class;
	return true;
}

}
