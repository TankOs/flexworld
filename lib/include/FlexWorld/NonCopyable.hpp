#pragma once

namespace flex {

/** Non-copyable base class.
 */
class NonCopyable {
	public:
		NonCopyable() = default;
		NonCopyable( const NonCopyable& other ) = delete;
		NonCopyable& operator=( const NonCopyable& other ) = delete;
};

}
