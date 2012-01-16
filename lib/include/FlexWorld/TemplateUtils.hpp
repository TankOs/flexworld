#pragma once

#include <limits>
#include <cstddef>

/// @cond NEVER

namespace flex {
namespace tpl {

struct None {};

template <class H, class T = None>
struct Typelist {
	typedef H Head; ///< Head.
	typedef T Tail; ///< Tail.
};

// Equal.
template <class A0, class A1>
struct Equal {
	static const bool RESULT = false;
};

template <class A0>
struct Equal<A0, A0> {
	static const bool RESULT = true;
};

// IfThenElse
template <bool Cond, std::size_t Then, std::size_t Else>
struct IfThenElse {
	static const std::size_t RESULT = Else;
};

template <std::size_t Then, std::size_t Else>
struct IfThenElse<true, Then, Else> {
	static const std::size_t RESULT = Then;
};

// IndexOf
template <class T, class TL>
struct IndexOf {
	static const std::size_t RESULT = IfThenElse<Equal<T, typename TL::Head>::RESULT, 0, IndexOf<T, typename TL::Tail>::RESULT + 1>::RESULT;
};

template <class T>
struct IndexOf<T, None> {
	static const std::size_t RESULT = 0;
};

}
}

#include "TemplateUtils.inl"

/// @endcond
