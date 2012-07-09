#pragma once

#include <FlexWorld/Exception.hpp>

#include <string>

namespace fw {

/** FlexWorld identifier.
 * The FlexWorld identifier (short FlexID or FID) is used to identify resources
 * of any kind. It has to be in the following format:
 *
 * package[.subpackage[...]][/resource]
 *
 * A package is composed of one or multiple groups of the characters
 * [a-zA-Z0-9], separated by dots. A package must not start with a dot.
 * Examples:
 *
 * - fw
 * - fw.weapons
 *
 * The resource is composed of the characters [a-zA-Z0-9_-.] and is optional.
 * It's delimited from the package with a slash. Examples:
 *
 * - sword.png
 * - dwarf.yml
 *
 * Complete FlexID examples:
 *
 * - fw.base/grass.yml
 * - fw.weapons/sword.png
 * - fw.monsters (this points to a package only)
 *
 * When a FlexID instance is created, its internal state is invalid. You either
 * have to set the package and optionally the resource or parse a full ID. The
 * constructor doesn't allow to set the values directly to avoid errors while
 * parsing.
 *
 * Alternatively one can use make() to directly parse a string at the cost of a
 * possible exception.
 */
class FlexID {
	public:
		/** Thrown when make() fails to parse.
		 */
		FLEX_MAKE_RUNTIME_ERROR_EXCEPTION( ParserException );

		/** Create FlexID by directly parsing an ID string.
		 * @param string String.
		 * @return FlexID.
		 * @throws ParserException if parsing fails.
		 */
		static FlexID make( const std::string& string );

		/** Ctor.
		 */
		FlexID();

		/** Set package.
		 * If the package is invalid the internal state is unchanged. If it's valid
		 * then the resource will be kept (if set).
		 * @param package Package.
		 * @return true if given package is valid, false otherwise.
		 */
		bool set_package( const std::string& package );

		/** Set resource.
		 * If the resource is invalid the internal state is unchanged.
		 * @param resource Resource.
		 * @return true if given resource is valid, false otherwise.
		 */
		bool set_resource( const std::string& resource );

		/** Parse a full ID.
		 * If parsing fails, the previous state is recovered.
		 * @param id ID.
		 * @return true on success.
		 */
		bool parse( const std::string& id );

		/** Check if package is valid.
		 * @return true if valid.
		 */
		bool is_valid_package() const;

		/** Check if resource is valid.
		 * @return true if valid.
		 */
		bool is_valid_resource() const;

		/** Get full ID.
		 * @return ID or empty if not set.
		 */
		std::string get() const;

		/** Get package.
		 * @return Package or empty if not set.
		 */
		const std::string& get_package() const;

		/** Get resource.
		 * @return Resource or empty if not set.
		 */
		const std::string& get_resource() const;

		/** Equal?
		 * @param other Other ID.
		 * @return true if equal.
		 */
		bool operator==( const FlexID& other ) const;

		/** Not equal?
		 * @param other Other ID.
		 * @return true if equal.
		 */
		bool operator!=( const FlexID& other ) const;

		/** Convert to path.
		 * A trailing slash is appended to package ID paths.
		 * @return Path.
		 */
		std::string as_path() const;

	private:
		std::string m_package;
		std::string m_resource;
};

}
