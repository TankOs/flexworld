#include <FlexWorld/SaveInfoDriver.hpp>

#include <yaml-cpp/yaml.h>
#include <sstream>

namespace flex {

SaveInfo SaveInfoDriver::deserialize( const std::string& buffer ) {
	std::stringstream sstr( buffer );
	YAML::Parser parser( sstr );

	YAML::Node doc;
	if( !parser.GetNextDocument( doc ) ) {
		throw DeserializeException( "No document." );
	}

	const YAML::Node* root_node( doc.FindValue( "Save" ) );
	if( !root_node ) {
		throw DeserializeException( "No root node." );
	}
	else if( root_node->Type() != YAML::NodeType::Map ) {
		throw DeserializeException( "Root not a map." );
	}

	SaveInfo info;

	// Read meta info.
	{
		const YAML::Node* meta_node( root_node->FindValue( "Meta" ) );
		if( !meta_node ) {
			throw DeserializeException( "No meta node." );
		}
		else if( meta_node->Type() != YAML::NodeType::Map ) {
			throw DeserializeException( "Meta not a map." );
		}

		// Read name.
		{
			const YAML::Node* name_node( meta_node->FindValue( "Name" ) );
			if( !name_node ) {
				throw DeserializeException( "No name." );
			}
			else if( name_node->Type() != YAML::NodeType::Scalar ) {
				throw DeserializeException( "Name not a scalar." );
			}

			std::string name;
			*name_node >> name;
			if( name.empty() ) {
				throw DeserializeException( "Name is empty." );
			}

			info.set_name( name );
		}

		// Read timestamp.
		{
			const YAML::Node* timestamp_node( meta_node->FindValue( "Timestamp" ) );
			if( !timestamp_node ) {
				throw DeserializeException( "No timestamp." );
			}
			else if( timestamp_node->Type() != YAML::NodeType::Scalar ) {
				throw DeserializeException( "Timestamp not a scalar." );
			}

			uint32_t timestamp;
			try {
				*timestamp_node >> timestamp;
			}
			catch( const YAML::Exception& e ) {
				throw DeserializeException( "Invalid timestamp." );
			}

			info.set_timestamp( timestamp );
		}
	}

	// Read paths.
	{
		const YAML::Node* paths_node( root_node->FindValue( "Paths" ) );
		if( !paths_node ) {
			throw DeserializeException( "No paths node." );
		}
		else if( paths_node->Type() != YAML::NodeType::Map ) {
			throw DeserializeException( "Paths not a map." );
		}

		// Read entities path.
		{
			const YAML::Node* entities_node( paths_node->FindValue( "Entities" ) );
			if( !entities_node ) {
				throw DeserializeException( "No entities path." );
			}
			else if( entities_node->Type() != YAML::NodeType::Scalar ) {
				throw DeserializeException( "Entities not a scalar." );
			}

			std::string path;
			*entities_node >> path;
			if( path.empty() ) {
				throw DeserializeException( "Entities path is empty." );
			}

			info.set_entities_path( path );
		}

		// Read planets path.
		{
			const YAML::Node* planets_node( paths_node->FindValue( "Planets" ) );
			if( !planets_node ) {
				throw DeserializeException( "No planets path." );
			}
			else if( planets_node->Type() != YAML::NodeType::Scalar ) {
				throw DeserializeException( "Planets not a scalar." );
			}

			std::string path;
			*planets_node >> path;
			if( path.empty() ) {
				throw DeserializeException( "Planets path is empty." );
			}

			info.set_planets_path( path );
		}

		// Read accounts path.
		{
			const YAML::Node* accounts_node( paths_node->FindValue( "Accounts" ) );
			if( !accounts_node ) {
				throw DeserializeException( "No accounts path." );
			}
			else if( accounts_node->Type() != YAML::NodeType::Scalar ) {
				throw DeserializeException( "Accounts not a scalar." );
			}

			std::string path;
			*accounts_node >> path;
			if( path.empty() ) {
				throw DeserializeException( "Accounts path is empty." );
			}

			info.set_accounts_path( path );
		}
	}

	return info;
}

std::string SaveInfoDriver::serialize( const SaveInfo& info ) {
	assert( !info.get_name().empty() );
	assert( !info.get_entities_path().empty() );
	assert( !info.get_planets_path().empty() );
	assert( !info.get_accounts_path().empty() );

	using namespace YAML;
	Emitter emitter;

	emitter
		<< BeginMap
			<< Key << "Save" << Value << BeginMap
				<< Key << "Meta" << Value << BeginMap
					<< Key << "Name" << Value << info.get_name()
					<< Key << "Timestamp" << Value << info.get_timestamp()
				<< EndMap
				<< Key << "Paths" << Value << BeginMap
					<< Key << "Entities" << Value << info.get_entities_path()
					<< Key << "Planets" << Value << info.get_planets_path()
					<< Key << "Accounts" << Value << info.get_accounts_path()
				<< EndMap
			<< EndMap
		<< EndMap
	;

	return emitter.c_str();
}

}
