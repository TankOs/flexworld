#include <FlexWorld/Messages/LoginOK.hpp>

#include <cstring>

namespace flex {
namespace msg {

LoginOK::LoginOK() :
	Message()
{
}

void LoginOK::serialize( Buffer& buffer ) const {
	std::size_t buf_ptr = buffer.size();

	// Enlarge buffer.
	buffer.resize(
		+ buf_ptr
		+ sizeof( uint8_t ) // Magic.
	);

	buffer[buf_ptr] = 0; ++buf_ptr;
}

std::size_t LoginOK::deserialize( const char* buffer, std::size_t buffer_size ) {
	std::size_t buf_ptr( 0 );

	// Magic.
	if( buffer_size - buf_ptr < sizeof( uint8_t ) ) {
		return 0;
	}

	uint8_t magic = buffer[buf_ptr];
	buf_ptr += sizeof( uint8_t );

	if( magic != 0 ) {
		throw BogusDataException( "Invalid magic." );
	}

	return buf_ptr;
}

}
}
