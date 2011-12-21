#pragma once

#include <string>

namespace flex {

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
 * * fw
 * * fw.weapons
 *
 * The resource is composed of the characters [a-zA-Z0-9_-.] and is optional.
 * It's delimited from the package with a slash. Examples:
 *
 * * sword.png
 * * dwarf.yml
 *
 * Complete FlexID examples:
 *
 * * fw.base/grass.yml
 * * fw.weapons/sword.png
 * * fw.monsters (this points to a package only)
 *
 * When a FlexID instance is created, its internal state is invalid. You either
 * have to set the package and optionally the resource or parse a full ID. The
 * constructor doesn't allow to set the values directly to avoid errors while
 * parsing.
 */
class FlexID {
	public:
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

		/** Check if ID is valid.
		 * A FlexID is valid if at least the package is set.
		 * @return true if valid.
		 */
		bool is_valid() const;

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

	private:
		bool parse( const std::string& id );

		std::string m_package;
		std::string m_resource;
};

}
