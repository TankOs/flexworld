#if !defined( NDEBUG )
	#include <typeinfo>
	#include <iostream>
#endif

namespace flex {

/// @cond NEVER

template <class TL, class ConnID>
void MessageHandler<TL, ConnID>::handle_message( const typename TL::Head& message, ConnID sender ) {
	#if !defined( NDEBUG )
		std::cerr << "Unhandled message from sender " << sender << ": " << typeid( message ).name() << std::endl;
	#endif
}

/// @endcond

}
