#include <FlexWorld/ClassDriver.hpp>
#include <FlexWorld/Class.hpp>

#include <yaml-cpp/yaml.h>
#include <fstream>

namespace flex {

void ClassDriver::load( const std::string& path, Class& cls ) {
	// Copy original file to keep ID.
	Class new_cls( cls );

	// Open file.
	std::ifstream in( path.c_str() );
	if( !in.is_open() ) {
		throw LoadException( "Failed to open file." );
	}

	// Setup parser.
	YAML::Parser parser( in );
	YAML::Node doc;

	parser.GetNextDocument( doc );

	std::string string;

	// Read name.
	{
		const YAML::Node* name_node( doc.FindValue( "Name" ) );
		if( !name_node ) {
			string = "Unnamed";
		}
		else if( name_node->Type() != YAML::NodeType::Scalar ) {
			throw LoadException( "Name not a scalar." );
		}

		*name_node >> string;
		if( string.empty() ) {
			throw LoadException( "Name empty." );
		}

		new_cls.set_name( string );
	}

	// Read model.
	{
		const YAML::Node* model_node( doc.FindValue( "Model" ) );
		if( !model_node ) {
			throw LoadException( "No model set." );
		}
		else if( model_node->Type() != YAML::NodeType::Scalar ) {
			throw LoadException( "Model not a scalar." );
		}

		*model_node >> string;

		FlexID id;
		if( !id.parse( string ) ) {
			throw LoadException( "Invalid ID." );
		}
		else if( !id.is_valid_resource() ) {
			throw LoadException( "Model is not a resource." );
		}

		new_cls.set_model( Resource( id ) );
	}

	// Read origin.
	const YAML::Node* origin_node = doc.FindValue( "Origin" );
	if( origin_node ) {
		if( origin_node->Type() != YAML::NodeType::Sequence ) {
			throw LoadException( "Origin not a sequence." );
		}
		else if( origin_node->size() != 3 ) {
			throw LoadException( "Invalid origin." );
		}

		sf::Vector3f origin( 0, 0, 0 );

		try {
			(*origin_node)[0] >> origin.x;
			(*origin_node)[1] >> origin.y;
			(*origin_node)[2] >> origin.z;
		}
		catch( const YAML::Exception& /*e*/ ) {
			throw LoadException( "Invalid origin value(s)." );
		}

		new_cls.set_origin( origin );
	}

	// Read scale.
	const YAML::Node* scale_node = doc.FindValue( "Scale" );
	if( scale_node ) {
		if( scale_node->Type() != YAML::NodeType::Sequence ) {
			throw LoadException( "Scale not a sequence." );
		}
		else if( scale_node->size() != 3 ) {
			throw LoadException( "Invalid scale." );
		}

		sf::Vector3f scale( 1, 1, 1 );

		try {
			(*scale_node)[0] >> scale.x;
			(*scale_node)[1] >> scale.y;
			(*scale_node)[2] >> scale.z;
		}
		catch( const YAML::Exception& /*e*/ ) {
			throw LoadException( "Invalid scale value(s)." );
		}

		new_cls.set_scale( scale );
	}

	// Read bounding box.
	const YAML::Node* b_box_node = doc.FindValue( "BoundingBox" );
	if( b_box_node ) {
		if( b_box_node->Type() != YAML::NodeType::Sequence ) {
			throw LoadException( "BoundingBox not a sequence." );
		}
		else if( b_box_node->size() != 6 ) {
			throw LoadException( "Invalid bounding box." );
		}

		FloatCuboid box( 0, 0, 0, 0, 0, 0 );

		try {
			(*b_box_node)[0] >> box.x;
			(*b_box_node)[1] >> box.y;
			(*b_box_node)[2] >> box.z;
			(*b_box_node)[3] >> box.width;
			(*b_box_node)[4] >> box.height;
			(*b_box_node)[5] >> box.depth;
		}
		catch( const YAML::Exception& /*e*/ ) {
			throw LoadException( "Invalid BoundingBox value(s)." );
		}

		if( box.width <= 0 || box.height <= 0 || box.depth <= 0 ) {
			throw LoadException( "Invalid width, height and/or depth for BoundingBox." );
		}

		new_cls.set_bounding_box( box );
	}

	// Read hooks.
	{
		const YAML::Node* hooks_node = doc.FindValue( "Hooks" );
		if( hooks_node ) {
			if( hooks_node->Type() != YAML::NodeType::Map ) {
				throw LoadException( "Hooks not a map." );
			}

			YAML::Iterator hook_iter = hooks_node->begin();
			YAML::Iterator hook_iter_end = hooks_node->end();
			std::string hook_name;
			sf::Vector3f hook_origin;

			for( ; hook_iter != hook_iter_end; ++hook_iter ) {
				hook_iter.first() >> hook_name;

				// Check for reserved name.
				if( hook_name.size() > 1 && hook_name[0] == '_' && hook_name[1] == '_' ) {
					throw LoadException( "Invalid use of reserved hook name." );
				}

				if( hook_iter.second().Type() != YAML::NodeType::Sequence ) {
					throw LoadException( "Hook not a sequence (" + hook_name + ")." );
				}
				else if( hook_iter.second().size() != 3 ) {
					throw LoadException( "Invalid hook origin (" + hook_name + ")." );
				}

				try {
					hook_iter.second()[0] >> hook_origin.x;
					hook_iter.second()[1] >> hook_origin.y;
					hook_iter.second()[2] >> hook_origin.z;
				}
				catch( const YAML::Exception& /*e*/ ) {
					throw LoadException( "Invalid hook origin value (" + hook_name + ")." );
				}

				new_cls.set_hook( hook_name, hook_origin );
			}
		}
	}

	// Read textures.
	{
		const YAML::Node* textures_node = doc.FindValue( "Textures" );
		if( textures_node ) {
			if( textures_node->Type() != YAML::NodeType::Sequence ) {
				throw LoadException( "Textures not a sequence." );
			}

			YAML::Iterator texture_iter = textures_node->begin();
			YAML::Iterator texture_iter_end = textures_node->end();
			std::string texture_id;

			for( ; texture_iter != texture_iter_end; ++texture_iter ) {
				if( texture_iter->Type() != YAML::NodeType::Scalar ) {
					throw LoadException( "A texture is not a scalar." );
				}

				*texture_iter >> texture_id;

				FlexID flex_id;
				if( !flex_id.parse( texture_id ) ) {
					throw LoadException( "Invalid texture ID (" + texture_id + ")." );
				}
				else if( !flex_id.is_valid_resource() ) {
					throw LoadException( "Texture ID is not a resource (" + texture_id + ")." );
				}

				new_cls.add_texture( Resource( flex_id ) );
			}
		}
	}

	// Read container image.
	{
		const YAML::Node* container_image_node( doc.FindValue( "ContainerImage" ) );

		if( container_image_node ) {
			if( container_image_node->Type() != YAML::NodeType::Scalar ) {
				throw LoadException( "ContainerImage not a scalar." );
			}

			*container_image_node >> string;

			FlexID id;
			if( !id.parse( string ) ) {
				throw LoadException( "Invalid ID." );
			}
			else if( !id.is_valid_resource() ) {
				throw LoadException( "ContainerImage is not a resource." );
			}

			new_cls.set_container_image( Resource( id ) );
		}
	}

	// Loading succeeded, copy loaded class to output parameter.
	cls = new_cls;
}

}
