#pragma once

#include "UserSettings.hpp"

#include <Rocket/Core/EventListener.h>
#include <memory>

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
};
