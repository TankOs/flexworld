#include <FlexWorld/NonCopyable.hpp>

namespace fw {

NonCopyable::NonCopyable() {
}

NonCopyable::NonCopyable( const NonCopyable& ) {
}

const NonCopyable& NonCopyable::operator=( const NonCopyable& ) {
	return *this;
}

}
