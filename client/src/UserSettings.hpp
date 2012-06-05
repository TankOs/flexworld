#pragma once

#include "Controls.hpp"
#include "TextureFilter.hpp"

#include <SFML/Window/VideoMode.hpp>
#include <SFML/System/Vector2.hpp>

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

		/** Get controls.
		 * @return Controls.
		 */
		const Controls& get_controls() const;

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

		/** Set video mode.
		 * @param mode Mode.
		 */
		void set_video_mode( const sf::VideoMode& mode );

		/** Get video mode.
		 * @return Mode.
		 */
		const sf::VideoMode& get_video_mode() const;

		/** Enable fullscreen.
		 * @param enable Enable?
		 */
		void enable_fullscreen( bool enable );

		/** Check if fullscreen enabled.
		 * @return true if enabled.
		 */
		bool is_fullscreen_enabled() const;

		/** Set window position.
		 * @param pos Position.
		 */
		void set_window_position( const sf::Vector2i& pos );

		/** Get window position.
		 * @return Position.
		 */
		const sf::Vector2i& get_window_position() const;

		/** Set anisotropy level.
		 * @param level Level.
		 */
		void set_anisotropy_level( uint8_t level );

		/** Get anisotropy level.
		 * @return Anisotropy level.
		 */
		uint8_t get_anisotropy_level() const;

		/** Set texture filter.
		 * @param filter Filter.
		 */
		void set_texture_filter( TextureFilter filter );

		/** Get texture filter.
		 * @return Filter.
		 */
		TextureFilter get_texture_filter() const;

	private:
		Controls m_controls;

		sf::VideoMode m_video_mode;

		std::string m_username;
		std::string m_serial;

		sf::Vector2i m_window_position;

		TextureFilter m_texture_filter;

		uint32_t m_fps_limit;
		uint8_t m_fov;
		uint8_t m_anisotropy_level;
		bool m_vsync;
		bool m_fullscreen;
};
