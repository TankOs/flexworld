#include <FlexWorld/ClassDriver.hpp>
#include <FlexWorld/Class.hpp>

#include <yaml-cpp/yaml.h>
#include <fstream>

namespace flex {

bool ClassDriver::load( const std::string& path, Class& cls ) {
	// Open file.
	std::ifstream in( path.c_str() );
	if( !in.is_open() ) {
		return false;
	}

	// Setup parser.
	YAML::Parser parser( in );
	YAML::Node doc;

	parser.GetNextDocument( doc );

	try {
		std::string string;
		doc["Id"] >> string;
		Class new_class( string );

		doc["Name"] >> string;
		new_class.set_name( string );

		// Everything okay.
		cls = new_class;
		return true;
	}
	catch( ... ) {
		return false;
	}

	// Will never reach this point.
	return false;
}

}
