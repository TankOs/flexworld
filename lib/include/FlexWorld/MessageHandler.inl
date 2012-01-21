#if !defined( NDEBUG )
	#include <typeinfo>
	#include <iostream>
#endif

namespace flex {

/// @cond NEVER

#if !defined( NDEBUG )
	template <class TL, class ConnID>
	void MessageHandler<TL, ConnID>::handle_message( const typename TL::Head& message, ConnID sender ) {
			std::cerr << "Unhandled message from sender " << sender << ": " << typeid( message ).name() << std::endl;
	}
#else
	template <class TL, class ConnID>
	void MessageHandler<TL, ConnID>::handle_message( const typename TL::Head& /*message*/, ConnID /*sender*/ ) {
	}
#endif

/// @endcond

}
