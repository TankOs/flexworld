#include "Shared.hpp"

Shared::Shared() {
}

Shared& Shared::get() {
	static Shared shared;
	return shared;
}

UserSettings& Shared::get_user_settings() {
	return m_user_settings;
}
