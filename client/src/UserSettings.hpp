#pragma once

#include "Controls.hpp"

#include <map>
#include <string>

/** User settings.
 */
class UserSettings {
	public:
		/** Ctor.
		 */
		UserSettings();

		/** Get user's profile path.
		 * @return Profile path.
		 */
		static std::string get_profile_path();

		/** Load.
		 * If loading fails, the previous settings are restored.
		 * @param filename Filename.
		 * @return true on success.
		 */
		bool load( const std::string& filename );

		/** Save.
		 * @param filename Filename.
		 * @return true on success.
		 */
		bool save( const std::string& filename );

		/** Get controls.
		 * @return Controls.
		 */
		Controls& get_controls();

		/** Set username.
		 * @param username Username.
		 */
		void set_username( const std::string& username );

		/** Get username.
		 * @return Username.
		 */
		const std::string& get_username() const;

		/** Set serial.
		 * @param serial Serial.
		 */
		void set_serial( const std::string& serial );

		/** Get serial.
		 * @return Serial.
		 */
		const std::string& get_serial() const;

	private:
		std::string m_username;
		std::string m_serial;

		Controls m_controls;
};
