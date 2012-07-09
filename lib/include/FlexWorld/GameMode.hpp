#pragma once

#include <FlexWorld/Version.hpp>
#include <FlexWorld/FlexID.hpp>

#include <vector>
#include <cstdint>

namespace fw {

/** Game mode.
 */
class GameMode {
	public:
		/** Ctor.
		 */
		GameMode();

		/** Set name.
		 * @param name Name.
		 */
		void set_name( const std::string& name );

		/** Set author.
		 * @param author Author.
		 */
		void set_author( const std::string& author );

		/** Set website URL.
		 * @param website Website URL.
		 */
		void set_website( const std::string& website );

		/** Set version.
		 * @param version Version.
		 */
		void set_version( const Version& version );

		/** Set minimum required FlexWorld version.
		 * @param version Version.
		 */
		void set_min_flexworld_version( const Version& version );

		/** Set maximum allowed Flexworld version.
		 * @param version Version.
		 */
		void set_max_flexworld_version( const Version& version );

		/** Set thumbnail.
		 * @param thumb Thumbnail (must be a valid resource).
		 */
		void set_thumbnail( const FlexID& thumb );

		/** Get name.
		 * @return Name.
		 */
		const std::string& get_name() const;

		/** Get author.
		 * @return Author.
		 */
		const std::string& get_author() const;

		/** Get website URL.
		 * @return Website URL.
		 */
		const std::string& get_website() const;

		/** Get version.
		 * @return Version.
		 */
		const Version& get_version() const;

		/** Get minimum FlexWorld version.
		 * @return Minimum FlexWorld version.
		 */
		const Version& get_min_flexworld_version() const;

		/** Get maximum FlexWorld version.
		 * @return Maximum FlexWorld version.
		 */
		const Version& get_max_flexworld_version() const;

		/** Get thumbnail.
		 * @return Thumbnail.
		 */
		const FlexID& get_thumbnail() const;

		/** Get number of packages.
		 * @return Number of packages.
		 */
		std::size_t get_num_packages() const;

		/** Get package.
		 * Undefined behaviour if index is invalid.
		 * @param index Index.
		 * @return Package.
		 * @see get_num_packages for checking amount of packages.
		 */
		const FlexID& get_package( std::size_t index ) const;

		/** Add package.
		 * @param package Package (must be a valid package ID).
		 */
		void add_package( const FlexID& package );

		/** Set default entity class ID.
		 * @param id Class ID.
		 */
		void set_default_entity_class_id( const FlexID& id );

		/** Get default entity class ID.
		 * @return Default entity class ID.
		 */
		const FlexID& get_default_entity_class_id();

	private:
		std::vector<FlexID> m_packages;
		FlexID m_thumbnail;
		FlexID m_default_entity_class_id;
		std::string m_name;
		std::string m_author;
		std::string m_website;
		Version m_version;
		Version m_min_flexworld_version;
		Version m_max_flexworld_version;
};

}
