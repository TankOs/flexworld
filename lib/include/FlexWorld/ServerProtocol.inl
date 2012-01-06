#include <cstring>
#include <cassert>

namespace flex {

template <class T>
void ServerProtocol::pack_byte( const T& value, Buffer& buffer ) {
	assert( sizeof( T ) == 1 );

	buffer.resize( buffer.size() + sizeof( T ) );
	std::memcpy( &buffer[buffer.size() - sizeof( T )], &value, sizeof( T ) );
}

template <class T>
void ServerProtocol::pack_word( const T& value, Buffer& buffer ) {
	assert( sizeof( T ) == 2 );

	buffer.resize( buffer.size() + sizeof( T ) );
	std::memcpy( &buffer[buffer.size() - sizeof( T )], &value, sizeof( T ) );
}

template <class T>
void ServerProtocol::pack_dword( const T& value, Buffer& buffer ) {
	assert( sizeof( T ) == 4 );

	buffer.resize( buffer.size() + sizeof( T ) );
	std::memcpy( &buffer[buffer.size() - sizeof( T )], &value, sizeof( T ) );
}

}
