#include <FlexWorld/AccountManager.hpp>
#include <FlexWorld/AccountDriver.hpp>

#include <boost/filesystem.hpp>
#include <fstream>
#include <sstream>
#include <cassert>

namespace fw {

AccountManager::AccountManager() {
}

const Account* AccountManager::find_account( const std::string& username ) const {
	assert( !username.empty() && "Invalid username." );

	AccountMap::const_iterator acc_iter( m_accounts.find( username ) );
	return acc_iter == m_accounts.end() ? nullptr : &acc_iter->second;
}

void AccountManager::add_account( const Account& account ) {
	assert( !account.get_username().empty() && "Invalid account" );
	assert( !account.get_password().empty() && "Invalid account" );
	assert( find_account( account.get_username() ) == nullptr && "Account already exists." );

	m_accounts[account.get_username()] = account;
}

std::size_t AccountManager::get_num_accounts() const {
	return m_accounts.size();
}

void AccountManager::remove_account( const std::string& username ) {
	assert( !username.empty() );

	AccountMap::iterator iter( m_accounts.find( username ) );
	assert( iter != m_accounts.end() );

	m_accounts.erase( iter );
}

void AccountManager::update_account( const Account& account ) {
	assert( !account.get_username().empty() );
	assert( !account.get_password().empty() );
	assert( find_account( account.get_username() ) != nullptr );

	m_accounts[account.get_username()] = account;
}

std::size_t AccountManager::load_accounts_from_directory( const std::string& path_, LoadStrategy strategy ) {
	using namespace boost::filesystem;

	assert( exists( path_ ) );
	assert( is_directory( path_ ) );

	path bpath( path_ );
	directory_iterator dir_iter( bpath );
	directory_iterator dir_iter_end;
	AccountMap new_accounts;

	for( ; dir_iter != dir_iter_end; ++dir_iter ) {
		path dir_path( *dir_iter );

		// Process files only.
		if( !is_regular( dir_path ) ) {
			continue;
		}

		// Check extension.
		if( dir_path.extension() != ".yml" ) {
			continue;
		}

		bool failed( false );

		// Try to load.
		try {
			// Load data.
			std::ifstream in( dir_path.c_str() );
			if( !in.is_open() ) {
				failed = true;
			}
			else {
				std::stringstream sstr;
				sstr << in.rdbuf();

				if( !sstr ) {
					failed = true;
				}
				else {
					Account acc = AccountDriver::deserialize( sstr.str() );
					new_accounts[acc.get_username()] = acc;
				}
			}
		}
		catch( const AccountDriver::DeserializeException& /*e*/ ) {
			failed = true;
		}

		if( failed && strategy == CANCEL_ON_FAILURE ) {
			return 0;
		}
	}

	// Swap and return amount of loaded accounts.
	std::swap( m_accounts, new_accounts );
	return m_accounts.size();
}

}
