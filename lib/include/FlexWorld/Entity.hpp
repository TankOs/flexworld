#pragma once

#include <FlexWorld/NonCopyable.hpp>

#include <SFML/System/Vector3.hpp>
#include <string>
#include <memory>
#include <cstdint>

namespace flex {

class Class;

/** Entity.
 * An entity is a dynamic object in FlexWorld: They can move, collide, rotate etc.
 */
class Entity {
	public:
		typedef uint32_t ID; ///< Entity ID type.
		typedef uint32_t AmountType; ///< Amount type.

		/** Ctor.
		 * @param cls Class.
		 */
		Entity( const Class& cls );

		/** Copy ctor.
		 * @param other Other entity.
		 */
		Entity( const Entity& other );

		/** Assignment.
		 * @param other Other entity.
		 * @return *this.
		 */
		Entity& operator=( const Entity& other );

		/** Get ID.
		 * @return ID.
		 */
		ID get_id() const;

		/** Set ID.
		 * @param id ID.
		 */
		void set_id( ID id );

		/** Get class.
		 * @return Class.
		 */
		const Class& get_class() const;

		/** Get name.
		 * If no custom name is set, the class' name is returned.
		 * @return Name.
		 */
		const std::string& get_name() const;

		/** Set name.
		 * If the name argument is empty, the classes name is NOT restored. Use
		 * reset_name() instead.
		 * @param name Name.
		 */
		void set_name( const std::string& name );

		/** Reset name to classes name.
		 */
		void reset_name();

		/** Get amount.
		 * @return Amount.
		 */
		AmountType get_amount() const;

		/** Set amount.
		 * @param amount Amount (must be greater than 0).
		 */
		void set_amount( AmountType amount );

		/** Get position.
		 * @return Position.
		 */
		const sf::Vector3f& get_position() const;

		/** Set position.
		 * @param position Position.
		 */
		void set_position( const sf::Vector3f& position );

		/** Set rotation.
		 * @param rotation Rotation.
		 */
		void set_rotation( const sf::Vector3f& rotation );

		/** Get rotation.
		 * @return Rotation.
		 */
		const sf::Vector3f& get_rotation() const;

	private:
		sf::Vector3f m_position;
		sf::Vector3f m_rotation;
		std::unique_ptr<std::string> m_name;
		ID m_id;
		AmountType m_amount;
		const Class* m_class;
};

}
