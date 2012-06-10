#pragma once

#include "UserSettings.hpp"

#include <Rocket/Core/EventListener.h>
#include <memory>
#include <functional>
#include <map>

namespace Rocket {
namespace Core {
class Element;
}
namespace Controls {
class ElementFormControlInput;
}
}

namespace sf {
class Event;
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

		/** Get user settings.
		 * @return User settings.
		 */
		const UserSettings& get_user_settings() const;

		std::function<void()> on_reject; ///< Called when window is closed and settings are NOT saved.
		std::function<void()> on_accept; ///< Called when window is closed and settings ARE saved.

	private:
		typedef std::map<Controls::Action, Rocket::Core::Element*> ActionElementMap;

		void ProcessEvent( Rocket::Core::Event& event );

		void update_sensitivity_number();
		void update_binding_labels();
		void create_binding_elements( const std::string& description = "", Controls::Action action = Controls::UNMAPPED );

		UserSettings m_user_settings;
		ActionElementMap m_action_elements;

		Rocket::Core::Element& m_root;

		Rocket::Controls::ElementFormControlInput* m_username_element;
		Rocket::Controls::ElementFormControlInput* m_serial_element;

		Rocket::Controls::ElementFormControlInput* m_invert_mouse_element;
		Rocket::Controls::ElementFormControlInput* m_sensitivity_element;
		Rocket::Core::Element* m_sensitivity_number_element;
		Rocket::Core::Element* m_bindings_element;

		Rocket::Core::Element* m_resolution_element;

		Rocket::Core::Element* m_close_element;
		Rocket::Core::Element* m_save_element;

		Controls::Action m_current_action_id;
		bool m_is_waiting_for_press;
};
