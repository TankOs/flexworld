#pragma once

#include <string>
#include <cstdint>

namespace flex {

/** Simple version structure.
 */
class Version {
	public:
		/** Ctor.
		 */
		Version( uint32_t major = 0, uint32_t minor = 0, uint32_t revision = 0 );

		/** Get major.
		 * @return Major.
		 */
		uint32_t get_major() const;

		/** Get minor.
		 * @return Minor.
		 */
		uint32_t get_minor() const;

		/** Get revision.
		 * @return Revision.
		 */
		uint32_t get_revision() const;

		/** Set major.
		 * @param major Major.
		 */
		void set_major( uint32_t major );

		/** Set minor.
		 * @param minor Minor.
		 */
		void set_minor( uint32_t minor );

		/** Set revision.
		 * @param revision Revision.
		 */
		void set_revision( uint32_t revision );

	private:
		uint32_t m_major;
		uint32_t m_minor;
		uint32_t m_revision;
};

/** Compare.
 * @param left Left.
 * @param right Right.
 * @return true if equal.
 */
bool operator==( const Version& left, const Version& right );

/** Compare.
 * @param left Left.
 * @param right Right.
 * @return false if equal.
 */

bool operator!=( const Version& left, const Version& right );

/** Check greater than.
 * @param left Left.
 * @param right Right.
 * @return true if left is greater.
 */
bool operator>( const Version& left, const Version& right );

/** Check less than.
 * @param left Left.
 * @param right Right.
 * @return true if left is less.
 */
bool operator<( const Version& left, const Version& right );

}
