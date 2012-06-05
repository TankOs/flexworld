#pragma once

#include "UserSettings.hpp"
#include "BindingDataSource.hpp"

#include <Rocket/Core/EventListener.h>
#include <memory>

namespace Rocket {
namespace Core {
class Element;
}
namespace Controls {
class ElementFormControlInput;
class ElementDataGrid;
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

		UserSettings m_user_settings;
		std::unique_ptr<BindingDataSource> m_bindings_data_source;

		Rocket::Core::Element& m_root;

		Rocket::Controls::ElementFormControlInput* m_username_element;
		Rocket::Controls::ElementFormControlInput* m_serial_element;

		Rocket::Controls::ElementFormControlInput* m_invert_mouse_element;
		Rocket::Controls::ElementFormControlInput* m_sensitivity_element;
		Rocket::Core::Element* m_sensitivity_number_element;
		Rocket::Controls::ElementDataGrid* m_bindings_data_grid;
};
