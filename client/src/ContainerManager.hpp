#pragma once

#include "Container.hpp"

#include <SFGUI/Signal.hpp>

#include <map>
#include <list>
#include <memory>
#include <cstdint>

namespace sf {
class RenderTarget;
}

//namespace sfg {
//class Desktop;
//}

/** Visual container manager.
 */
class ContainerManager {
	public:
		/** Ctor.
		 */
		ContainerManager();

		/** Create container.
		 * If a container with the same idea was created before (and closed again),
		 * the position will be restored.
		 * @param id ID.
		 * @return Created container.
		 */
		Container& create_container( uint32_t id );

		/** Destroy container.
		 * Undefined behaviour if container with ID doesn't exist.
		 * @param id Container ID.
		 */
		void destroy_container( uint32_t id );

		/** Get active container.
		 * Undefined behaviour if there're no containers.
		 * @return Active container.
		 */
		Container& get_active_container();

		/** Find container.
		 * @param id ID.
		 * @return Container or nullptr.
		 */
		Container* find_container( uint32_t id );

		/** Render.
		 * @param target Render target.
		 */
		void render( sf::RenderTarget& target ) const;

		/** Handle SFML event.
		 * @param event Event.
		 */
		void handle_event( const sf::Event& event );

		/** Check if last event got eaten.
		 * @return true if eaten.
		 */
		bool was_event_eaten() const;

		/** Get number of containers.
		 * @return Number of containers.
		 */
		std::size_t get_num_containers() const;

		/** Clear.
		 * All containers are removed. Saved positions remain.
		 */
		void clear();

		sfg::Signal on_container_destroy; ///< Fired when a container is destroyed.

	private:
		typedef std::map<uint32_t, Container> ContainerMap;
		typedef std::map<uint32_t, sf::Vector2f> ContainerPositionMap;
		typedef std::list<Container*> ContainerPtrList;

		ContainerMap m_containers;
		ContainerPtrList m_stacked_containers;
		ContainerPositionMap m_saved_positions;

		sf::Vector2f m_dragging_delta;

		bool m_event_eaten;
		bool m_prepare_dragging;
		bool m_dragging;

		//sfg::Desktop& m_desktop;
};
