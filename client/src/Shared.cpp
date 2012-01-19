#include "Shared.hpp"

Shared::Shared() {
}

Shared& Shared::get() {
	static Shared shared;
	return shared;
}

Shared& get_shared() {
	return Shared::get();
}
