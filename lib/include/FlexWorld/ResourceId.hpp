#pragma once

#include <string>
#include <stdexcept>

namespace flex {

/** Resource identifier.
 */
class ResourceId {
	public:
		/** Parser exception.
		 */
		class ParserException : public std::runtime_error {
			public:
				/** Ctor.
				 * @param msg What.
				 */
				ParserException( const std::string& msg );
		};

		/** Ctor.
		 * @param id ID to parse.
		 */
		ResourceId( const std::string& id );

		/** Get full ID.
		 * @return ID.
		 */
		std::string get() const;

		/** Get package ID.
		 * @return Package ID or empty if invalid.
		 */
		const std::string& get_package_id() const;

		/** Get path.
		 * @return Path or empty if invalid.
		 */
		const std::string& get_path() const;

		/** Equal?
		 * @param other Other ID.
		 * @return true if equal.
		 */
		bool operator==( const ResourceId& other ) const;

		/** Not equal?
		 * @param other Other ID.
		 * @return true if equal.
		 */
		bool operator!=( const ResourceId& other ) const;

	private:
		bool parse( const std::string& id );

		std::string m_package;
		std::string m_path;
};

}
