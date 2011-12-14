#include <FlexWorld/NonCopyable.hpp>

namespace flex {

NonCopyable::NonCopyable() {
}

NonCopyable::NonCopyable( const NonCopyable& ) {
}

const NonCopyable& NonCopyable::operator=( const NonCopyable& ) {
	return *this;
}

}
