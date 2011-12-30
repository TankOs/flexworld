#include <FlexWorld/ClassDriver.hpp>
#include <FlexWorld/Class.hpp>

#include <yaml-cpp/yaml.h>
#include <fstream>

namespace flex {

Class ClassDriver::load( const std::string& path ) {
	// Open file.
	std::ifstream in( path.c_str() );
	if( !in.is_open() ) {
		throw LoadException( "Failed to open file." );
	}

	// Setup parser.
	YAML::Parser parser( in );
	YAML::Node doc;

	parser.GetNextDocument( doc );

	std::unique_ptr<Class> cls;
	std::string string;

	// Read ID.
	try {
		doc["ID"] >> string;

		FlexID id;
		if( !id.parse( string ) ) {
			throw LoadException( "Invalid ID." );
		}

		cls.reset( new Class( id ) );
	}
	catch( const YAML::Exception& e ) {
		throw LoadException( "Failed to parse ID." );
	}

	// Read name.
	try {
		doc["Name"] >> string;
		cls->set_name( string );
	}
	catch( const YAML::Exception& e ) {
		throw LoadException( "Failed to parse name." );
	}

	// Read origin.
	const YAML::Node* origin_node = doc.FindValue( "Origin" );
	if( origin_node ) {
		if( origin_node->size() != 3 ) {
			throw LoadException( "Invalid origin." );
		}

		sf::Vector3f origin( 0, 0, 0 );

		try {
			(*origin_node)[0] >> origin.x;
			(*origin_node)[1] >> origin.y;
			(*origin_node)[2] >> origin.z;
		}
		catch( const YAML::Exception& e ) {
			throw LoadException( "Invalid origin value(s)." );
		}

		cls->set_origin( origin );
	}

	// Read hooks.
	const YAML::Node* hooks_node = doc.FindValue( "Hooks" );
	if( hooks_node ) {
		YAML::Iterator hook_iter = hooks_node->begin();
		YAML::Iterator hook_iter_end = hooks_node->end();
		std::string hook_name;
		sf::Vector3f hook_origin;

		for( ; hook_iter != hook_iter_end; ++hook_iter ) {
			hook_iter.first() >> hook_name;

			if( hook_iter.second().size() != 3 ) {
				throw LoadException( "Invalid hook origin (" + hook_name + ")." );
			}

			try {
				hook_iter.second()[0] >> hook_origin.x;
				hook_iter.second()[1] >> hook_origin.y;
				hook_iter.second()[2] >> hook_origin.z;
			}
			catch( const YAML::Exception& e ) {
				throw LoadException( "Invalid hook origin value (" + hook_name + ")." );
			}

			cls->set_hook( hook_name, hook_origin );
		}
	}

	// Read textures.
	const YAML::Node* textures_node = doc.FindValue( "Textures" );
	if( textures_node ) {
		YAML::Iterator texture_iter = textures_node->begin();
		YAML::Iterator texture_iter_end = textures_node->end();
		std::string texture_id;

		for( ; texture_iter != texture_iter_end; ++texture_iter ) {
			*texture_iter >> texture_id;

			FlexID flex_id;
			if( !flex_id.parse( texture_id ) ) {
				throw LoadException( "Invalid texture ID (" + texture_id + ")." );
			}

			cls->add_texture( Resource( flex_id ) );
		}
	}

	return *cls;
}

}
