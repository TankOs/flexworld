#pragma once

namespace Rocket {
namespace Core {
class Element;
}
}

/** Controller for the GUI chat window.
 */
class ChatWindowController {
	public:
		/** Ctor.
		 * @param root Root element of the chat window.
		 */
		ChatWindowController( Rocket::Core::Element* root );

		/** Dtor.
		 */
		~ChatWindowController();

	private:
		Rocket::Core::Element* m_root;
};
