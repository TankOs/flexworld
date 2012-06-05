#include "BindingDataSource.hpp"
#include "Controls.hpp"
#include "KeyNames.hpp"

#include <map>
#include <vector>
#include <string>
#include <cassert>

typedef std::map<int, Controls::Action> RowActionMap;

const std::string SEPARATOR_LINE = "-----------------------";

std::vector<std::string> ACTION_NAMES;
RowActionMap ACTION_MAPPINGS;

BindingDataSource::BindingDataSource( const Controls& controls ) :
	Rocket::Controls::DataSource( "bindings" ),
	m_controls( controls )
{
	if( ACTION_NAMES.size() == 0 ) {
		ACTION_MAPPINGS[static_cast<int>( ACTION_NAMES.size() )] = Controls::WALK_FORWARD; ACTION_NAMES.push_back( "Walk forward" );
		ACTION_MAPPINGS[static_cast<int>( ACTION_NAMES.size() )] = Controls::WALK_BACKWARD; ACTION_NAMES.push_back( "Walk backward" );
		ACTION_MAPPINGS[static_cast<int>( ACTION_NAMES.size() )] = Controls::STRAFE_LEFT; ACTION_NAMES.push_back( "Strafe left" );
		ACTION_MAPPINGS[static_cast<int>( ACTION_NAMES.size() )] = Controls::STRAFE_RIGHT; ACTION_NAMES.push_back( "Strafe right" );
		ACTION_MAPPINGS[static_cast<int>( ACTION_NAMES.size() )] = Controls::JUMP; ACTION_NAMES.push_back( "Jump" );
		ACTION_MAPPINGS[static_cast<int>( ACTION_NAMES.size() )] = Controls::CROUCH; ACTION_NAMES.push_back( "Crouch" );
		ACTION_MAPPINGS[static_cast<int>( ACTION_NAMES.size() )] = Controls::RUN; ACTION_NAMES.push_back( "Run" );
		ACTION_NAMES.push_back( SEPARATOR_LINE.c_str() );
		ACTION_MAPPINGS[static_cast<int>( ACTION_NAMES.size() )] = Controls::PRIMARY_ACTION; ACTION_NAMES.push_back( "Primary action" );
		ACTION_MAPPINGS[static_cast<int>( ACTION_NAMES.size() )] = Controls::SECONDARY_ACTION; ACTION_NAMES.push_back( "Secondary action" );
		ACTION_MAPPINGS[static_cast<int>( ACTION_NAMES.size() )] = Controls::USE; ACTION_NAMES.push_back( "Use (hold to pick up)" );
		ACTION_MAPPINGS[static_cast<int>( ACTION_NAMES.size() )] = Controls::DROP; ACTION_NAMES.push_back( "Drop" );
		ACTION_NAMES.push_back( SEPARATOR_LINE.c_str() );
		ACTION_MAPPINGS[static_cast<int>( ACTION_NAMES.size() )] = Controls::INVENTORY; ACTION_NAMES.push_back( "Toggle desk" );
		ACTION_MAPPINGS[static_cast<int>( ACTION_NAMES.size() )] = Controls::CHAT; ACTION_NAMES.push_back( "Chat" );
	}
}

int BindingDataSource::GetNumRows( const Rocket::Core::String& /*table*/ ) {
	return static_cast<int>( ACTION_NAMES.size() );
}

void BindingDataSource::GetRow( Rocket::Core::StringList& row, const Rocket::Core::String& /*table*/, int row_index, const Rocket::Core::StringList& columns ) {
	for( std::size_t col_idx = 0; col_idx < columns.size(); ++col_idx ) {
		if( columns[col_idx] == "function" ) {
			assert( static_cast<std::size_t>( row_index ) < ACTION_NAMES.size() );

			row.push_back( ACTION_NAMES[row_index].c_str() );
		}
		else if( columns[col_idx] == "binding" ) {
			// Empty if there's no real mapping.
			RowActionMap::const_iterator iter = ACTION_MAPPINGS.find( row_index );

			if( iter == ACTION_MAPPINGS.end() ) {
				row.push_back( "" );
			}
			else {
				// Find out which button or key is bound to action.
				Controls::Action action = iter->second;
				std::string readable_name = "";

				Controls::ControlMap::const_iterator key_iter = m_controls.keys_begin();
				Controls::ControlMap::const_iterator key_iter_end = m_controls.keys_end();

				for( ; key_iter != key_iter_end; ++key_iter ) {
					if( key_iter->second == action ) {
						readable_name = get_key_name( key_iter->first );
						break;
					}
				}

				// Key not found, continue with mouse buttons.
				if( readable_name.empty() ) {
					Controls::ControlMap::const_iterator button_iter = m_controls.buttons_begin();
					Controls::ControlMap::const_iterator button_iter_end = m_controls.buttons_end();

					for( ; button_iter != button_iter_end; ++button_iter ) {
						if( button_iter->second == action ) {
							readable_name = get_button_name( button_iter->first );
							break;
						}
					}
				}

				// If not key/button mapping found then the action is unmapped.
				if( readable_name.empty() ) {
					readable_name = "UNMAPPED";
				}

				row.push_back( readable_name.c_str() );
			}
		}
	}
}
