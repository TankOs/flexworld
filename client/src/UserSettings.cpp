#include "UserSettings.hpp"

#include <yaml-cpp/yaml.h>
#include <fstream>

#if defined( LINUX ) || defined( WINDOWS )
	#include <cstdlib>
#endif

const uint8_t UserSettings::MAX_FOV = 120;
const uint8_t UserSettings::MIN_FOV = 60;

std::string UserSettings::get_profile_path() {
#if defined( LINUX )
	return std::string( std::getenv( "HOME" ) ) + "/.flexworld";
#elif defined( WINDOWS )
	return std::string( std::getenv( "APPDATA" ) ) + "/flexworld";
#else
	#error "MISSING CODE :-("
#endif
}

UserSettings::UserSettings() :
	m_video_mode( sf::VideoMode::getDesktopMode() ),
	m_fps_limit( 60 ),
	m_fov( 60 ),
	m_vsync( false ),
	m_fullscreen( true )
{
}

bool UserSettings::load( const std::string& filename ) {
	std::ifstream in_stream( filename.c_str() );

	if( !in_stream.is_open() ) {
		return false;
	}

	UserSettings new_settings;

	YAML::Parser parser( in_stream );
	YAML::Node doc;

	if( !parser.GetNextDocument( doc ) ) {
		return false;
	}

	try { doc["Account"]["Username"] >> new_settings.m_username; } catch( ... ) {}
	try { doc["Account"]["Serial"] >> new_settings.m_serial; } catch( ... ) {}

	try {
		bool inverted( new_settings.m_controls.is_mouse_inverted() );
		doc["Controls"]["Mouse"]["Inverted"] >> inverted;
		new_settings.m_controls.set_mouse_inverted( inverted );
	}
	catch( ... ) {
	}

	try {
		float sens( new_settings.m_controls.get_mouse_sensitivity() );
		doc["Controls"]["Mouse"]["Sensitivity"] >> sens;
		new_settings.m_controls.set_mouse_sensitivity( sens );
	}
	catch( ... ) {
	}

	// Video settings.
	const YAML::Node* video_node = doc.FindValue( "Video" );

	if( video_node ) {
		const YAML::Node* resolution_node = video_node->FindValue( "Resolution" );

		if( resolution_node && resolution_node->Type() == YAML::NodeType::Sequence && resolution_node->size() == 3 ) {
			int width = 0;
			int height = 0;
			int bpp = 0;

			try {
				(*resolution_node)[0] >> width;
				(*resolution_node)[1] >> height;
				(*resolution_node)[2] >> bpp;

				new_settings.set_video_mode( sf::VideoMode( width, height, bpp ) );
			}
			catch( const YAML::Exception& /*e*/ ) {
			}
		}

		const YAML::Node* fullscreen_node = video_node->FindValue( "Fullscreen" );

		if( fullscreen_node ) {
			try {
				bool enable = true;

				*fullscreen_node >> enable;

				new_settings.enable_fullscreen( enable );
			}
			catch( const YAML::Exception& /*e*/ ) {
			}
		}

		const YAML::Node* vsync_node = video_node->FindValue( "VSync" );

		if( vsync_node ) {
			try {
				bool enable = false;

				*vsync_node >> enable;
				new_settings.enable_vsync( enable );
			}
			catch( const YAML::Exception& ) {
			}
		}

		const YAML::Node* fps_node = video_node->FindValue( "FPSLimit" );

		if( fps_node ) {
			try {
				int fps_limit = 60;

				*fps_node >> fps_limit;

				if( fps_limit > 0 ) {
					new_settings.set_fps_limit( static_cast<uint32_t>( fps_limit ) );
				}
			}
			catch( const YAML::Exception& ) {
			}
		}

		const YAML::Node* fov_node = video_node->FindValue( "FOV" );

		if( fov_node ) {
			try {
				int read = 60;

				*fov_node >> read;
				uint8_t fov = static_cast<uint8_t>( read );

				fov = std::min( MAX_FOV, std::max( MIN_FOV, static_cast<uint8_t>( fov ) ) );
				new_settings.set_fov( fov );
			}
			catch( const YAML::Exception& ) {
			}
		}

		const YAML::Node* window_position_node = video_node->FindValue( "WindowPosition" );

		if( window_position_node ) {
			try {
				sf::Vector2i position;

				(*window_position_node)[0] >> position.x;
				(*window_position_node)[1] >> position.y;

				if( position.x >= 0 && position.y >= 0 ) {
					new_settings.set_window_position( position );
				}
			}
			catch( const YAML::Exception& ) {
			}
		}
	}

	// Load key bindings.
	try {
		const YAML::Node& key_bindings_node( doc["Controls"]["Keyboard"]["Bindings"] );
		YAML::Iterator key_iter( key_bindings_node.begin() );
		YAML::Iterator key_iter_end( key_bindings_node.end() );

		for( ; key_iter != key_iter_end; ++key_iter ) {
			int key( 0 );
			int action( 0 );
			key_iter.first() >> key;
			key_iter.second() >> action;

			if( action > Controls::UNMAPPED && action < Controls::ACTION_COUNT ) {
				new_settings.m_controls.map_key( key, static_cast<Controls::Action>( action ) );
			}
		}
	}
	catch( ... ) {
	}

	// Load button bindings.
	try {
		const YAML::Node& button_bindings_node( doc["Controls"]["Mouse"]["Bindings"] );
		YAML::Iterator button_iter( button_bindings_node.begin() );
		YAML::Iterator button_iter_end( button_bindings_node.end() );

		for( ; button_iter != button_iter_end; ++button_iter ) {
			int button( 0 );
			int action( 0 );
			button_iter.first() >> button;
			button_iter.second() >> action;

			if( action > Controls::UNMAPPED && action < Controls::ACTION_COUNT ) {
				new_settings.m_controls.map_button( button, static_cast<Controls::Action>( action ) );
			}
		}
	}
	catch( ... ) {
	}

	// Loading succeeded, apply loaded settings.
	*this = new_settings;
	return true;
}

bool UserSettings::save( const std::string& filename ) {
	using namespace YAML;

	std::ofstream out_stream( filename.c_str() );

	if( !out_stream.is_open() ) {
		return false;
	}

	Emitter emitter;
	emitter.SetSeqFormat( Flow );
	emitter
		<< BeginMap
			<< Key << "Account" << Value << BeginMap;

				if( !m_username.empty() ) {
					emitter << Key << "Username" << Value << m_username;
				}

				if( !m_serial.empty() ) {
					emitter << Key << "Serial" << Value << m_serial;
				}

			emitter << EndMap
			<< Key << "Video" << Value << BeginMap
				<< Key << "Resolution" << Value << BeginSeq
					<< static_cast<int>( get_video_mode().width )
					<< static_cast<int>( get_video_mode().height )
					<< static_cast<int>( get_video_mode().bitsPerPixel )
				<< EndSeq
				<< Key << "Fullscreen" << Value << is_fullscreen_enabled()
				<< Key << "VSync" << Value << is_vsync_enabled()
				<< Key << "FPSLimit" << Value << get_fps_limit()
				<< Key << "FOV" << Value << get_fov()
				<< Key << "WindowPosition" << Value << BeginSeq
					<< get_window_position().x << get_window_position().y
				<< EndSeq
			<< EndMap
			<< Key << "Controls" << Value << BeginMap
				<< Key << "Mouse" << Value << BeginMap
					<< Key << "Inverted" << Value << m_controls.is_mouse_inverted()
					<< Key << "Sensitivity" << Value << m_controls.get_mouse_sensitivity()
					<< Key << "Bindings" << Value << BeginMap;

						Controls::ControlMap::const_iterator button_iter( m_controls.buttons_begin() );
						Controls::ControlMap::const_iterator button_iter_end( m_controls.buttons_end() );
						
						for( ; button_iter != button_iter_end; ++button_iter ) {
							emitter << Key << button_iter->first << Value << button_iter->second;
						}

					emitter << EndMap
				<< EndMap
				<< Key << "Keyboard" << Value << BeginMap
					<< Key << "Bindings" << Value << BeginMap;

						Controls::ControlMap::const_iterator key_iter( m_controls.keys_begin() );
						Controls::ControlMap::const_iterator key_iter_end( m_controls.keys_end() );
						
						for( ; key_iter != key_iter_end; ++key_iter ) {
							emitter << Key << key_iter->first << Value << key_iter->second;
						}

					emitter << EndMap
				<< EndMap
			<< EndMap
		<< EndMap
	;

	out_stream << emitter.c_str();
	return true;
}

Controls& UserSettings::get_controls() {
	return m_controls;
}

void UserSettings::set_username( const std::string& username ) {
	m_username = username;
}

const std::string& UserSettings::get_username() const {
	return m_username;
}

void UserSettings::set_serial( const std::string& serial ) {
	m_serial = serial;
}

const std::string& UserSettings::get_serial() const {
	return m_serial;
}

void UserSettings::enable_vsync( bool enable ) {
	m_vsync = enable;
}

bool UserSettings::is_vsync_enabled() const {
	return m_vsync;
}

void UserSettings::set_fps_limit( uint32_t limit ) {
	assert( limit > 0 );
	m_fps_limit = limit;
}

uint32_t UserSettings::get_fps_limit() const {
	return m_fps_limit;
}

void UserSettings::set_fov( uint8_t fov ) {
	m_fov = fov;
}

uint8_t UserSettings::get_fov() const {
	return m_fov;
}

void UserSettings::set_video_mode( const sf::VideoMode& mode ) {
	m_video_mode = mode;
}

const sf::VideoMode& UserSettings::get_video_mode() const {
	return m_video_mode;
}

void UserSettings::enable_fullscreen( bool enable ) {
	m_fullscreen = enable;
}

bool UserSettings::is_fullscreen_enabled() const {
	return m_fullscreen;
}

void UserSettings::set_window_position( const sf::Vector2i& pos ) {
	m_window_position = pos;
}

const sf::Vector2i& UserSettings::get_window_position() const {
	return m_window_position;
}
