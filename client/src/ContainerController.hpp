#pragma once

#include <string>

namespace fw {
class Entity;
}

namespace Rocket {
namespace Core {
class Element;
}
}

/** Controller for container GUI elements.
 */
class ContainerController {
	public:
		/** Ctor.
		 * @param root Root container element.
		 */
		ContainerController( Rocket::Core::Element& root );

		/** Copy ctor.
		 * @param other Other.
		 */
		ContainerController( const ContainerController& other ) = delete;

		/** Assignment.
		 * @param other Other.
		 */
		ContainerController& operator=( const ContainerController& other ) = delete;

		/** Set background image.
		 * @param filename Filename.
		 */
		void set_background_image( const std::string& filename );

		/** Add entity.
		 * @param entity Entity.
		 */
		void add_entity( const fw::Entity& entity );

		/** Remove entity.
		 * @param entity Entity.
		 */
		void remove_entity( const fw::Entity& entity );

		/** Update entity.
		 * Call this if the position, name or whatever of an entity has changed.
		 * Undefined behaviour if entity hasn't been added before.
		 * @param entity Entity.
		 */
		void update_entity( const fw::Entity& entity );

	private:
		Rocket::Core::Element& m_root_element;
		Rocket::Core::Element* m_background_element;
		Rocket::Core::Element* m_container_element;
};
