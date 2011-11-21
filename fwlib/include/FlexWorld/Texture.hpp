#pragma once

#include <string>

namespace flex {

/** Texture.
 * This class doesn't represent a visual texture, but properties about it (for
 * loading and using it).
 */
class Texture {
	public:
		/** Ctor.
		 */
		Texture();

		/** Set path.
		 * @param path Path.
		 */
		void set_path( const std::string& path );

		/** Get path.
		 * @return Path.
		 */
		const std::string& get_path() const;

	private:
		std::string m_path;
};

}
