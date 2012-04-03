#pragma once

#include <SFGUI/Window.hpp>
#include <SFGUI/Label.hpp>
#include <SFGUI/Entry.hpp>
#include <SFGUI/Button.hpp>
#include <SFGUI/ScrolledWindow.hpp>
#include <SFGUI/Box.hpp>
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
		void CreateChannel( const sf::String& name );

		/** Focus entry of active page.
		 */
		void FocusEntry();

		/** Clear input entry.
		 */
		void ClearEntry();

		/** Get message the user wants to send.
		 * Call this after OnMessageReady was fired.
		 * @return Message.
		 */
		const sf::String& GetMessage() const;

		/** Get number of channels.
		 * @return Number of channels.
		 */
		std::size_t GetNumChannels() const;

		/** Get channel name.
		 * @param index Channel index (must be valid).
		 * @return Channel name.
		 */
		const sf::String& GetChannelName( std::size_t index ) const;

		/** Get active channel.
		 * @return Channel index.
		 */
		std::size_t GetActiveChannel() const;

		/** Add message.
		 * Channel will be automagically created if it doesn't exist.
		 * @param message Message.
		 * @param channel Channel.
		 */
		void AddMessage( const sf::String& message, const sf::String& channel );

		sfg::Signal OnMessageReady; ///< Fired when user wants to send a message.
		sfg::Signal OnCloseClick; ///< Fired when user clicks on "Close" button to close the window.

	private:
		struct Channel {
			sf::String name;
			sfg::ScrolledWindow::Ptr scrolled_window;
			sfg::Box::Ptr vbox;
		};

		typedef std::vector<Channel> ChannelArray;

		ChatWindow();

		Channel* FindChannel( const sf::String& name );

		void OnSendButtonClick();
		void OnInputEntryKeyPress();
		void OnCloseButtonClick();

		ChannelArray m_channels;

		sfg::Notebook::Ptr m_notebook;
		sfg::Entry::Ptr m_input_entry;
		sfg::Button::Ptr m_send_button;
		sfg::Button::Ptr m_clear_button;
		sfg::Button::Ptr m_join_button;
		sfg::Button::Ptr m_close_button;

		sf::String m_message;
};
