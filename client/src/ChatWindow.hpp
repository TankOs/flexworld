#pragma once

#include <SFGUI/Window.hpp>
#include <SFGUI/Label.hpp>
#include <SFGUI/Entry.hpp>
#include <SFGUI/Button.hpp>
#include <SFGUI/ScrolledWindow.hpp>
#include <SFGUI/Notebook.hpp>

/** Window for providing chat UI with multiple channels.
 */
class ChatWindow : public sfg::Window {
	public:
		typedef sfg::SharedPtr<ChatWindow> Ptr; ///< Shared pointer.
		typedef sfg::SharedPtr<const ChatWindow> PtrConst; ///< Shared pointer to cinst.

		/** Create chat window.
		 * @return Newly created chat window.
		 */
		static Ptr Create();

		/** Create channel.
		 * Skipped if channel does already exist.
		 * @param name Name.
		 */
		void CreateChannel( const std::string& name );

		/** Focus entry of active page.
		 */
		void FocusEntry();

		/** Clear input entry.
		 */
		void ClearEntry();

	private:
		struct Channel {
			std::string name;
			sfg::ScrolledWindow::Ptr scrolled_window;
		};

		typedef std::vector<Channel> ChannelArray;

		ChatWindow();

		Channel* FindChannel( const std::string& name );

		ChannelArray m_channels;

		sfg::Notebook::Ptr m_notebook;
		sfg::Entry::Ptr m_input_entry;
		sfg::Button::Ptr m_send_button;
};
