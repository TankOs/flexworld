#pragma once

#include "UserSettings.hpp"

#include <Rocket/Core/EventListener.h>
#include <memory>
#include <functional>

namespace Rocket {
namespace Core {
class Element;
}
namespace Controls {
class ElementFormControlInput;
}
}

/** Controller for libRocket options document.
 */
class OptionsDocumentController : public Rocket::Core::EventListener {
	public:
		/** Ctor.
		 * @param root Root element.
		 */
		OptionsDocumentController( Rocket::Core::Element& root );

		/** Dtor.
		 */
		~OptionsDocumentController();

		/** Serialize user settings.
		 * @param user_settings User settings.
		 */
		void serialize( const UserSettings& user_settings );

		/** Is document waiting for a key/button?
		 * @return true if it's waiting.
		 */
		bool is_waiting_for_press() const;

		/** Handle event.
		 * The event is needed to save new bindings. Only needed if
		 * is_waiting_for_press() returns true.
		 * @param event Event.
		 */
		void handle_event( const sf::Event& event );

		std::function<void()> on_close; ///< Called when window is closed and settings are NOT saved.

	private:
		void ProcessEvent( Rocket::Core::Event& event );

		void update_sensitivity_number();
		void create_binding_elements( const std::string& description = "", Controls::Action action = Controls::UNMAPPED );

		UserSettings m_user_settings;

		Rocket::Core::Element& m_root;

		Rocket::Controls::ElementFormControlInput* m_username_element;
		Rocket::Controls::ElementFormControlInput* m_serial_element;

		Rocket::Controls::ElementFormControlInput* m_invert_mouse_element;
		Rocket::Controls::ElementFormControlInput* m_sensitivity_element;
		Rocket::Core::Element* m_sensitivity_number_element;
		Rocket::Core::Element* m_bindings_element;

		Rocket::Core::Element* m_close_element;

		bool m_is_waiting_for_press;
};
