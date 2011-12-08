#include <FlexWorld/AccountDriver.hpp>
#include <FlexWorld/Account.hpp>

#include <sstream>
#include <yaml-cpp/yaml.h>

namespace flex {

std::string AccountDriver::serialize( const Account& account ) {
	using namespace YAML;

	Emitter emitter;

	emitter
		<< BeginMap
			<< Key << "Username" << Value << account.get_username()
			<< Key << "Password" << Value << account.get_password()
		<< EndMap
	;

	return emitter.c_str();
}

bool AccountDriver::deserialize( const std::string& data, Account& account ) {
	using namespace YAML;

	std::stringstream stream( data );
	Parser parser( stream );
	Account new_account;

	try {
		Node doc;
		parser.GetNextDocument( doc );

		std::string buffer;
		doc["Username"] >> buffer;
		new_account.set_username( buffer );

		doc["Password"] >> buffer;
		new_account.set_password( buffer );
	}
	catch( ... ) {
		return false;
	}

	account = new_account;
	return true;
}

}
