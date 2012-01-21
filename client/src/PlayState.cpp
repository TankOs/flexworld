#include "PlayState.hpp"
#include "MenuState.hpp"
#include "Shared.hpp"

PlayState::PlayState( sf::RenderWindow& target ) :
	State( target )
{
}

void PlayState::init() {
	// Reset handler.
	get_shared().client->set_handler( *this );
}

void PlayState::cleanup() {
	// Close connections and wait for threads to complete.
	if( get_shared().client->is_connected() ) {
		get_shared().client->stop();
	}

	if( get_shared().client_thread ) {
		get_shared().client_thread->join();
	}

	if( get_shared().host ) {
		if( get_shared().host->is_running() ) {
			get_shared().host->stop();
		}

		if( get_shared().host_thread ) {
			get_shared().host_thread->join();
		}
	}

	// Free all backend stuff.
	get_shared().client.reset();
	get_shared().lock_facility.reset();
	get_shared().client_thread.reset();
	get_shared().host_thread.reset();
}

void PlayState::handle_event( const sf::Event& event ) {
	if(
		event.Type == sf::Event::Closed ||
		(event.Type == sf::Event::KeyPressed && event.Key.Code == sf::Keyboard::Escape)
	) {
		leave( new MenuState( get_render_target() ) );
	}
}

void PlayState::update( const sf::Time& delta ) {
}

void PlayState::render() const {
	sf::RenderWindow& target = get_render_target();

	target.Clear( sf::Color( 0x12, 0x34, 0x56 ) );
	target.Display();
}
