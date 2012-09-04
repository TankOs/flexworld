#pragma once

#include <FWMS/Reader.hpp>

namespace cs {
class System;
}

/** (FW) Component system reader.
 * Reader responsible for keeping the component system in sync and modifying it
 * depending on actions.
 */
class ComponentSystemReader : public ms::Reader {
	public:
		/** Ctor.
		 */
		ComponentSystemReader();

		/** Set system.
		 * @param system System (referenced).
		 */
		void set_system( cs::System& system );

	private:
		void handle_message( const ms::Message& message );

		cs::System* m_system;
};
