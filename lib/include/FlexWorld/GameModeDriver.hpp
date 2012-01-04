#pragma once

#include <FlexWorld/GameMode.hpp>
#include <FlexWorld/Exception.hpp>

#include <string>

namespace flex {

/** Driver for (de)serializing game modes from and to YAML.
 */
class GameModeDriver {
	public:
		/** Thrown when deserializing fails.
		 */
		FLEX_MAKE_RUNTIME_ERROR_EXCEPTION( DeserializeException );

		/** Deserialize game mode.
		 * @param buffer Buffer with game mode data.
		 * @return Game mode.
		 * @throws DeserializeException when parsing fails.
		 */
		static GameMode deserialize( const std::string& buffer );

	private:
};

}
