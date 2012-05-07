#include "ContainerManager.hpp"

#include <SFML/Window/Event.hpp>

#include <cassert>

ContainerManager::ContainerManager() :
	m_event_eaten( false ),
	m_prepare_dragging( false ),
	m_dragging( false )
{
}

Container& ContainerManager::create_container( uint32_t id ) {
	assert( m_containers.find( id ) == m_containers.end() );

	m_containers[id] = Container( id );
	m_stacked_containers.push_back( &m_containers[id] );

	// Check for saved position.
	ContainerPositionMap::const_iterator pos_iter = m_saved_positions.find( id );

	if( pos_iter != m_saved_positions.end() ) {
		m_containers[id].set_position( pos_iter->second );
	}

	return m_containers[id];
}

void ContainerManager::render( sf::RenderTarget& target ) const {
	ContainerPtrList::const_iterator cont_iter( m_stacked_containers.begin() );
	ContainerPtrList::const_iterator cont_iter_end( m_stacked_containers.end() );
	
	for( ; cont_iter != cont_iter_end; ++cont_iter ) {
		(*cont_iter)->render( target );
	}
}

void ContainerManager::handle_event( const sf::Event& event ) {
	m_event_eaten = false;

	if( m_containers.size() == 0 ) {
		return;
	}

	// Skip uninteresting events.
	if(
		event.type != sf::Event::MouseMoved &&
		event.type != sf::Event::MouseButtonPressed &&
		event.type != sf::Event::MouseButtonReleased
	) {
		return;
	}

	// Get container of interest.
	ContainerPtrList::iterator cont_iter( m_stacked_containers.begin() );
	ContainerPtrList::iterator cont_iter_end( m_stacked_containers.end() );
	Container* hit_container = nullptr;
	sf::Vector2f mouse_position = sf::Vector2f(
		event.type == sf::Event::MouseMoved ? static_cast<float>( event.mouseMove.x ) : static_cast<float>( event.mouseButton.x ),
		event.type == sf::Event::MouseMoved ? static_cast<float>( event.mouseMove.y ) : static_cast<float>( event.mouseButton.y )
	);
	
	for( ; cont_iter != cont_iter_end; ++cont_iter ) {
		Container& container = **cont_iter;

		sf::FloatRect cont_rect(
			container.get_position().x,
			container.get_position().y,
			container.get_size().x,
			container.get_size().y
		);

		if( cont_rect.contains( mouse_position ) ) {
			hit_container = &container;
			break;
		}
	}

	// If nothing hit, skip.
	if( !hit_container ) {
		return;
	}

	// Process left button presses.
	if( event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left ) {
		// If hit container isn't active, make it active.
		if( hit_container != m_stacked_containers.front() ) {
			// Bring to front.
			ContainerPtrList::iterator new_active_iter = std::find( m_stacked_containers.begin(), m_stacked_containers.end(), hit_container );
			assert( new_active_iter != m_stacked_containers.end() );

			m_stacked_containers.erase( new_active_iter );
			m_stacked_containers.push_front( hit_container );
		}

		// Prepare dragging.
		m_prepare_dragging = true;

		m_event_eaten = true;
	}

	// Process button releases.
	if( event.type == sf::Event::MouseButtonReleased ) {
		// Stop dragging.
		if( m_dragging || m_prepare_dragging ) {
			// Save position.
			if( m_dragging ) {
				m_saved_positions[hit_container->get_id()] = hit_container->get_position();
			}

			m_dragging = false;
			m_prepare_dragging = false;

			m_event_eaten = true;
		}
	}

	// Process mouse movement.
	if( event.type == sf::Event::MouseMoved ) {
		// If dragging was prepared, finally enable it.
		if( m_prepare_dragging ) {
			m_dragging = true;
			m_prepare_dragging = false;

			// Save delta between mouse position and top left corner of container.
			m_dragging_delta.x = mouse_position.x - hit_container->get_position().x;
			m_dragging_delta.y = mouse_position.y - hit_container->get_position().y;
		}

		// If dragging in progress, move container.
		if( m_dragging ) {
			hit_container->set_position(
				sf::Vector2f(
					mouse_position.x - m_dragging_delta.x,
					mouse_position.y - m_dragging_delta.y
				)
			);

			m_event_eaten = true;
		}

	}

}

bool ContainerManager::was_event_eaten() const {
	return m_event_eaten;
}

Container* ContainerManager::find_container( uint32_t id ) {
	ContainerMap::iterator iter = m_containers.find( id );

	return iter == m_containers.end() ? nullptr : &iter->second;
}

std::size_t ContainerManager::get_num_containers() const {
	return m_containers.size();
}

void ContainerManager::clear() {
	m_stacked_containers.clear();
	m_containers.clear();

	m_dragging = false;
	m_prepare_dragging = false;
	m_event_eaten = false;
}
