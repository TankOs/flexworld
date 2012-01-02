#pragma once

#include <string>

namespace flex {

/** SaveInfo contains useful information about a world save like paths and a descriptive name.
 */
class SaveInfo {
	public:
		/** Ctor.
		 */
		SaveInfo();

		/** Set name.
			* @param name New name.
			*/
		void set_name( const std::string& name );

		/** Set entities path.
			* @param entities_path New entities path.
			*/
		void set_entities_path( const std::string& entities_path );

		/** Set planets path.
			* @param planets_path New planets path.
			*/
		void set_planets_path( const std::string& planets_path );

		/** Set accounts path.
			* @param accounts_path New accounts path.
			*/
		void set_accounts_path( const std::string& accounts_path );

		/** Set timestamp.
			* @param timestamp New timestamp.
			*/
		void set_timestamp( uint32_t timestamp );

		/** Get name.
			* @return name.
			*/
		const std::string& get_name() const;

		/** Get entities path.
			* @return entities path.
			*/
		const std::string& get_entities_path() const;

		/** Get planets path.
			* @return planets path.
			*/
		const std::string& get_planets_path() const;

		/** Get accounts path.
			* @return accounts path.
			*/
		const std::string& get_accounts_path() const;

		/** Get timestamp.
			* @return timestamp.
			*/
		uint32_t get_timestamp() const;

	private:
		std::string m_name;
		std::string m_entities_path;
		std::string m_planets_path;
		std::string m_accounts_path;
		uint32_t m_timestamp;
};

}
