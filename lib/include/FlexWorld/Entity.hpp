#pragma once

#include <FlexWorld/NonCopyable.hpp>

#include <SFML/System/Vector3.hpp>
#include <string>
#include <vector>
#include <map>
#include <cstdint>

namespace flex {

class Class;

/** Entity.
 * An entity is a dynamic object in FlexWorld: They can move, collide, rotate etc.
 */
class Entity : public NonCopyable {
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
		//Entity( const Entity& other );

		/** Dtor.
		 */
		~Entity();

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

		/** Get parent.
		 * @return Parent or nullptr.
		 */
		const Entity* get_parent() const;

		/** Set parent.
		 * @param parent Parent or nullptr when no parent.
		 */
		void set_parent( const Entity* parent );

		/** Attach entity to hook.
		 * Undefined behaviour if entity is already attached to another entity or
		 * if hook is unknown.
		 * @param child Entity to attach, ptr is stored (must be in detached state).
		 * @param hook_id Hook ID (must exist).
		 */
		void attach( Entity& child, const std::string& hook_id );

		/** Detach entity.
		 * Undefined behaviour if not attached.
		 * @param child Child.
		 */
		void detach( Entity& child );

		/** Get number of total children.
		 * @return Number of children.
		 */
		std::size_t get_num_children() const;

		/** Get number of children at a specific hook.
		 * Undefined behaviour if hook doesn't exist.
		 * @param hook_id Hook (must exist).
		 * @return Number of children for hook.
		 */
		std::size_t get_num_children( const std::string& hook_id ) const;

		/** Check if child is attached.
		 * @param child Child.
		 * @return true if attached.
		 */
		bool has_child( const Entity& child ) const;

		/** Check if child is attached to specific hook.
		 * Undefined behaviour if hook ID is invalid.
		 * @param child Child.
		 * @param hook_id Hook (must exist).
		 * @return true if attached.
		 */
		bool has_child( const Entity& child, const std::string& hook_id ) const;

		/** Get child.
		 * Undefined behaviour if hook ID is invalid, no entity has been attached
		 * to the given hook or the index is invalid.
		 * @param hook_id Hook.
		 * @param index Index.
		 * @return Child.
		 */
		Entity& get_child( const std::string& hook_id, std::size_t index ) const;

	private:
		typedef std::vector<Entity*> EntityPtrArray;
		typedef std::map<const std::string, EntityPtrArray> HookEntityMap;

		/** Assignment.
		 * @param other Other entity.
		 * @return *this.
		 */
		Entity& operator=( const Entity& other );

		sf::Vector3f m_position;
		sf::Vector3f m_rotation;

		ID m_id;
		AmountType m_amount;

		HookEntityMap* m_children;
		std::string* m_name;
		const Class* m_class;
		const Entity* m_parent;
};

}
