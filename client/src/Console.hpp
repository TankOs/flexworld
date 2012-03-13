#pragma once

#include <SFGUI/SFGUI.hpp>

/** Console.
 */
class Console : public sfg::Window {
	public:
		typedef sfg::SharedPtr<Console> Ptr; ///< Shared pointer.
		typedef sfg::SharedPtr<const Console> PtrConst; ///< Shared pointer to const.

		/** Create console.
		 * @return Console.
		 */
		static Ptr Create();

		/** Add message.
		 * @param msg Message.
		 */
		void add_message( const sf::String& msg );

		/** Get number of messages.
		 * @return Number of messages.
		 */
		std::size_t get_num_messages() const;

		/** Get message.
		 * @param index Index.
		 * @return Message.
		 */
		const sf::String& get_message( std::size_t index ) const;

		sfg::Signal OnMessageAdd; ///< Fired when message is added.

	private:
		Console();

		sfg::ScrolledWindow::Ptr m_scrolled_window;
		sfg::Box::Ptr m_lines_box;
		sfg::Entry::Ptr m_command_entry;
		sfg::Button::Ptr m_ok_button;
};
