#pragma once

#include <SFGUI/Window.hpp>
#include <map>

namespace sfg {
class Button;
}

class DebugWindow : public sfg::Window {
	public:
		typedef sfg::SharedPtr<DebugWindow> Ptr; ///< Shared pointer.
		typedef sfg::SharedPtr<const DebugWindow> PtrConst; ///< Shared pointer to const.

		/** Create window.
		 * @return Window.
		 */
		static Ptr Create();

		/** Get current selected ID.
		 * @return Current selected ID.
		 */
		const std::string& GetCurrentID() const;

		sfg::Signal OnClassIDChange; ///< Fired when class ID changed.

	private:
		typedef std::map<sfg::SharedPtr<sfg::Button>, std::string> ButtonStringMap;

		DebugWindow();

		void HandleClassClick();

		ButtonStringMap m_buttons;
		std::string m_current_id;
};
