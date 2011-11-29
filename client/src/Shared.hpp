#pragma once

#include "UserSettings.hpp"

/** Shared singleton.
 */
class Shared {
	public:
		/** Get Shared instance.
		 * @return Shared instance.
		 */
		static Shared& get();

		/** Get user settings.
		 * @return User settings.
		 */
		UserSettings& get_user_settings();

	private:
		Shared();

		UserSettings m_user_settings;
};
