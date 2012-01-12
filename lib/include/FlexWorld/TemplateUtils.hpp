#pragma once

namespace flex {
namespace tpl {

/** Empty.
*/
struct None {};

/** Typelist.
 */
template <class H, class T = None>
struct Typelist {
	typedef H Head; ///< Head.
	typedef T Tail; ///< Tail.
};

}
}

#include "TemplateUtils.inl"
