#include <FlexWorld/AccountDriver.hpp>
#include <FlexWorld/Account.hpp>

#include <sstream>
#include <yaml-cpp/yaml.h>

namespace fw {

std::string AccountDriver::serialize( const Account& account ) {
	using namespace YAML;

	assert( account.get_username().empty() == false );
	assert( account.get_password().empty() == false );

	Emitter emitter;

	emitter
		<< BeginMap
			<< Key << "Username" << Value << account.get_username()
			<< Key << "Password" << Value << account.get_password()
			<< Key << "Entity" << Value << account.get_entity_id()
		<< EndMap
	;

	return emitter.c_str();
}

Account AccountDriver::deserialize( const std::string& data ) {
	using namespace YAML;

	std::stringstream stream( data );
	Parser parser( stream );
	Account new_account;

	YAML::Node doc;
	if( !parser.GetNextDocument( doc ) ) {
		throw DeserializeException( "No document." );
	}
	else if( doc.Type() != YAML::NodeType::Map ) {
		throw DeserializeException( "Root not a map." );
	}

	// Read username.
	{
		const YAML::Node* username_node = doc.FindValue( "Username" );
		if( !username_node ) {
			throw DeserializeException( "No username specified." );
		}
		else if( username_node->Type() != YAML::NodeType::Scalar ) {
			throw DeserializeException( "Username not a scalar." );
		}

		std::string username;
		*username_node >> username;

		if( username.empty() ) {
			throw DeserializeException( "Username empty." );
		}

		new_account.set_username( username );
	}

	// Read password.
	{
		const YAML::Node* password_node = doc.FindValue( "Password" );
		if( !password_node ) {
			throw DeserializeException( "No password specified." );
		}
		else if( password_node->Type() != YAML::NodeType::Scalar ) {
			throw DeserializeException( "Password not a scalar." );
		}

		std::string password;
		*password_node >> password;

		if( password.empty() ) {
			throw DeserializeException( "Password empty." );
		}

		new_account.set_password( password );
	}

	// Read entity ID.
	{
		const YAML::Node* entity_id_node = doc.FindValue( "Entity" );
		if( !entity_id_node ) {
			throw DeserializeException( "No entity ID specified." );
		}
		else if( entity_id_node->Type() != YAML::NodeType::Scalar ) {
			throw DeserializeException( "Entity ID not a scalar." );
		}

		Entity::ID entity_id;
		*entity_id_node >> entity_id;

		new_account.set_entity_id( entity_id );
	}

	return new_account;
}

}
