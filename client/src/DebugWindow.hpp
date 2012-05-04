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

		/** Get current spawn ID.
		 * @return Current spawn ID.
		 */
		const std::string& GetCurrentSpawnID() const;

		sfg::Signal OnClassIDChange; ///< Fired when class ID changed.
		sfg::Signal OnSpawnIDChange; ///< Fired when spawn class ID changed.

	private:
		typedef std::map<sfg::SharedPtr<sfg::Button>, std::string> ButtonStringMap;

		DebugWindow();

		void HandleClassClick();
		void HandleSpawnClick();

		ButtonStringMap m_buttons;
		ButtonStringMap m_spawn_buttons;
		std::string m_current_id;
		std::string m_current_spawn_id;
};
