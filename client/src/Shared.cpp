#include "Shared.hpp"

Shared::Shared() :
	entity_id( 0 )
{
}

Shared& Shared::get() {
	static Shared shared;
	return shared;
}

Shared& get_shared() {
	return Shared::get();
}
