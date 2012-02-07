#include <FlexWorld/GameModeDriver.hpp>

#include <yaml-cpp/yaml.h>
#include <memory>

namespace flex {

GameMode GameModeDriver::deserialize( const std::string& buffer ) {
	std::stringstream str( buffer );
	YAML::Node doc;

	try {
		YAML::Parser parser( str );

		if( !parser.GetNextDocument( doc ) ) {
			throw DeserializeException( "Document missing." );
		}
		else if( doc.Type() != YAML::NodeType::Map ) {
			throw DeserializeException( "Root not a map." );
		}
	}
	catch( ... ) {
		throw DeserializeException( "Malformed file." );
	}

	GameMode mode;

	// Meta.
	{
		const YAML::Node* meta_node( doc.FindValue( "Meta" ) );
		if( !meta_node ) {
			throw DeserializeException( "Meta info missing." );
		}
		else if( meta_node->Type() != YAML::NodeType::Map ) {
			throw DeserializeException( "Meta not a map." );
		}

		// Name.
		{
			const YAML::Node* name_node( meta_node->FindValue( "Name" ) );
			if( !name_node ) {
				throw DeserializeException( "Name missing." );
			}
			else if( name_node->Type() != YAML::NodeType::Scalar ) {
				throw DeserializeException( "Name not a scalar." );
			}

			std::string name;
			*name_node >> name;
			if( name.empty() ) {
				throw DeserializeException( "Name is empty." );
			}

			mode.set_name( name );
		}

		// Author.
		{
			const YAML::Node* author_node( meta_node->FindValue( "Author" ) );
			if( !author_node ) {
				throw DeserializeException( "Author missing." );
			}
			else if( author_node->Type() != YAML::NodeType::Scalar ) {
				throw DeserializeException( "Author not a scalar." );
			}

			std::string author;
			*author_node >> author;
			if( author.empty() ) {
				throw DeserializeException( "Author is empty." );
			}

			mode.set_author( author );
		}

		// Website.
		{
			const YAML::Node* website_node( meta_node->FindValue( "Website" ) );
			if( !website_node ) {
				throw DeserializeException( "Website missing." );
			}
			else if( website_node->Type() != YAML::NodeType::Scalar ) {
				throw DeserializeException( "Website not a scalar." );
			}

			std::string website;
			*website_node >> website;
			if( website.empty() ) {
				throw DeserializeException( "Website is empty." );
			}

			mode.set_website( website );
		}

		// Version.
		{
			const YAML::Node* version_node( meta_node->FindValue( "Version" ) );
			if( !version_node ) {
				throw DeserializeException( "Version missing." );
			}
			else if( version_node->Type() != YAML::NodeType::Sequence ) {
				throw DeserializeException( "Version not a sequence." );
			}
			else if( version_node->size() != 3 ) {
				throw DeserializeException( "Version has wrong size." );
			}

			uint32_t elements[3];

			try {
				(*version_node)[0] >> elements[0];
				(*version_node)[1] >> elements[1];
				(*version_node)[2] >> elements[2];
			}
			catch( const YAML::Exception& /*e*/ ) {
				throw DeserializeException( "Invalid version values." );
			}

			Version version;
			version.set_major( elements[0] );
			version.set_minor( elements[1] );
			version.set_revision( elements[2] );

			mode.set_version( version );
		}

		// Min FlexWorld version.
		{
			const YAML::Node* min_flexworld_version_node( meta_node->FindValue( "MinFlexWorldVersion" ) );
			if( !min_flexworld_version_node ) {
				throw DeserializeException( "MinFlexWorldVersion missing." );
			}
			else if( min_flexworld_version_node->Type() != YAML::NodeType::Sequence ) {
				throw DeserializeException( "MinFlexWorldVersion not a sequence." );
			}
			else if( min_flexworld_version_node->size() != 3 ) {
				throw DeserializeException( "MinFlexWorldVersion has wrong size." );
			}

			uint32_t elements[3];

			try {
				(*min_flexworld_version_node)[0] >> elements[0];
				(*min_flexworld_version_node)[1] >> elements[1];
				(*min_flexworld_version_node)[2] >> elements[2];
			}
			catch( const YAML::Exception& /*e*/ ) {
				throw DeserializeException( "Invalid MinFlexWorldVersion values." );
			}

			Version min_flexworld_version;
			min_flexworld_version.set_major( elements[0] );
			min_flexworld_version.set_minor( elements[1] );
			min_flexworld_version.set_revision( elements[2] );

			mode.set_min_flexworld_version( min_flexworld_version );
		}

		// Max FlexWorld version.
		{
			const YAML::Node* max_flexworld_version_node( meta_node->FindValue( "MaxFlexWorldVersion" ) );
			if( !max_flexworld_version_node ) {
				throw DeserializeException( "MaxFlexWorldVersion missing." );
			}
			else if( max_flexworld_version_node->Type() != YAML::NodeType::Sequence ) {
				throw DeserializeException( "MaxFlexWorldVersion not a sequence." );
			}
			else if( max_flexworld_version_node->size() != 3 ) {
				throw DeserializeException( "MaxFlexWorldVersion has wrong size." );
			}

			uint32_t elements[3];

			try {
				(*max_flexworld_version_node)[0] >> elements[0];
				(*max_flexworld_version_node)[1] >> elements[1];
				(*max_flexworld_version_node)[2] >> elements[2];
			}
			catch( const YAML::Exception& /*e*/ ) {
				throw DeserializeException( "Invalid MaxFlexWorldVersion values." );
			}

			Version max_flexworld_version;
			max_flexworld_version.set_major( elements[0] );
			max_flexworld_version.set_minor( elements[1] );
			max_flexworld_version.set_revision( elements[2] );

			mode.set_max_flexworld_version( max_flexworld_version );
		}

		// Thumbnail (optional).
		{
			const YAML::Node* thumbnail_node( meta_node->FindValue( "Thumbnail" ) );
			if( thumbnail_node ) {
				if( thumbnail_node->Type() != YAML::NodeType::Scalar ) {
					throw DeserializeException( "Thumbnail not a scalar." );
				}

				std::string thumbnail;
				*thumbnail_node >> thumbnail;
				if( thumbnail.empty() ) {
					throw DeserializeException( "Thumbnail is empty." );
				}

				FlexID thumb_id;
				if( !thumb_id.parse( thumbnail ) || !thumb_id.is_valid_resource() ) {
					throw DeserializeException( "Invalid thumbnail." );
				}

				mode.set_thumbnail( thumb_id );
			}
		}
	}

	// Packages.
	{
		const YAML::Node* packages_node( doc.FindValue( "Packages" ) );
		if( !packages_node ) {
			throw DeserializeException( "Packages missing." );
		}
		else if( packages_node->Type() != YAML::NodeType::Sequence ) {
			throw DeserializeException( "Packages not a sequence." );
		}

		for( std::size_t index = 0; index < packages_node->size(); ++index ) {
			const YAML::Node& package_node( (*packages_node)[index] );
			if( package_node.Type() != YAML::NodeType::Scalar ) {
				throw DeserializeException( "A package is not a scalar." );
			}

			std::string package;
			package_node >> package;

			FlexID id;
			if( !id.parse( package ) || !id.is_valid_package() || id.is_valid_resource() ) {
				throw DeserializeException( "Package invalid (" + package + ")." );
			}

			mode.add_package( id );
		}
	}

	return mode;
}

}
