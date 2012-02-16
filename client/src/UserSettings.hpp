#pragma once

#include "Controls.hpp"

#include <cstdint>
#include <string>

/** User settings.
 */
class UserSettings {
	public:
		static const uint8_t MAX_FOV; ///< Maximum FOV.
		static const uint8_t MIN_FOV; ///< Minimum FOV.

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

		/** Enable or disable vsync.
		 * @param enable Enable?
		 */
		void enable_vsync( bool enable );

		/** Check if vsync is enabled.
		 * @return true if enabled.
		 */
		bool is_vsync_enabled() const;

		/** Set FPS limit.
		 * @param limit Limit.
		 */
		void set_fps_limit( uint32_t limit );

		/** Get FPS limit.
		 * @return Limit.
		 */
		uint32_t get_fps_limit() const;

		/** Set FOV.
		 * @param fov FOV.
		 */
		void set_fov( uint8_t fov );

		/** Get FOV.
		 * @return FOV.
		 */
		uint8_t get_fov() const;

	private:
		Controls m_controls;

		std::string m_username;
		std::string m_serial;

		uint32_t m_fps_limit;

		uint8_t m_fov;
		bool m_vsync;
};
