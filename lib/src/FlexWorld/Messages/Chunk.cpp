#include <FlexWorld/Messages/Chunk.hpp>

#include <algorithm>
#include <cstring>
#include <cassert>

namespace flex {
namespace msg {

Chunk::Chunk() :
	Message()
{
}

void Chunk::serialize( Buffer& buffer ) const {
	std::size_t buf_ptr = buffer.size();

	// Check for valid amount of class IDs.
	if( m_class_ids.size() == 0 || m_class_ids.size() > 0x0fff ) {
		throw InvalidDataException( "Invalid amount of class IDs." );
	}

	// Check required amount of blocks.
	if( m_blocks.size() == 0 || m_blocks.size() > 0xffff ) {
		throw InvalidDataException( "Missing block data." );
	}

	// Count length of all class IDs.
	std::size_t total_class_id_length = 0;

	for( std::size_t class_id_idx = 0; class_id_idx < m_class_ids.size(); ++class_id_idx ) {
		// Check for valid class ID length.
		if( m_class_ids[class_id_idx].size() == 0 || m_class_ids[class_id_idx].size() > 255 ) {
			throw InvalidDataException( "Invalid class ID length of class " + m_class_ids[class_id_idx] );
		}

		total_class_id_length += m_class_ids[class_id_idx].size();
	}

	// Enlarge buffer.
	buffer.resize(
		+ buf_ptr
		+ sizeof( uint16_t ) // Number of class IDs.
		+ sizeof( uint8_t ) * m_class_ids.size() // Class ID lengths.
		+ total_class_id_length
		+ sizeof( uint16_t ) // Number of blocks.
		+ sizeof( uint16_t ) * m_blocks.size() // Block data.
	);

	*reinterpret_cast<uint16_t*>( &buffer[buf_ptr] ) = static_cast<uint16_t>( m_class_ids.size() );
	buf_ptr += sizeof( uint16_t );

	// Pack class IDs.
	for( std::size_t class_id_idx = 0; class_id_idx < m_class_ids.size(); ++class_id_idx ) {
		// Length.
		buffer[buf_ptr] = static_cast<uint8_t>( m_class_ids[class_id_idx].size() );
		buf_ptr += sizeof( uint8_t );

		// ID.
		std::memcpy( &buffer[buf_ptr], m_class_ids[class_id_idx].c_str(), m_class_ids[class_id_idx].size() );
		buf_ptr += m_class_ids[class_id_idx].size();
	}

	// Pack number of blocks.
	*reinterpret_cast<uint16_t*>( &buffer[buf_ptr] ) = static_cast<uint16_t>( m_blocks.size() );
	buf_ptr += sizeof( uint16_t );

	// Pack block data.
	for( std::size_t block_idx = 0; block_idx < m_blocks.size(); ++block_idx ) {
		*reinterpret_cast<uint16_t*>( &buffer[buf_ptr] ) = m_blocks[block_idx];
		buf_ptr += sizeof( uint16_t );
	}
}

std::size_t Chunk::deserialize( const char* buffer, std::size_t buffer_size ) {
	std::size_t buf_ptr( 0 );

	// Number of class IDs.
	if( buffer_size - buf_ptr < sizeof( uint16_t ) ) {
		return 0;
	}

	uint16_t num_class_ids = *reinterpret_cast<const uint16_t*>( &buffer[buf_ptr] );
	buf_ptr += sizeof( uint16_t );

	if( num_class_ids == 0 || num_class_ids > 0x0fff ) {
		throw BogusDataException( "Invalid number of class IDs." );
	}

	// Read class IDs one by one.
	std::vector<std::pair<const char*, std::size_t> > class_id_ptrs;

	for( std::size_t class_id_idx = 0; class_id_idx < num_class_ids; ++class_id_idx ) {
		// Class ID length.
		if( buffer_size - buf_ptr < sizeof( uint8_t ) ) {
			return 0;
		}

		uint8_t class_id_length = *reinterpret_cast<const uint8_t*>( &buffer[buf_ptr] );
		buf_ptr += sizeof( class_id_length );

		if( class_id_length == 0 ) {
			throw BogusDataException( "Invalid class ID length." );
		}

		// Class ID.
		if( buffer_size - buf_ptr < class_id_length ) {
			return 0;
		}

		// Store length + ID.
		class_id_ptrs.push_back( std::pair<const char*, std::size_t>( &buffer[buf_ptr], class_id_length ) );
		buf_ptr += class_id_length;
	}

	// Read number of blocks.
	if( buffer_size - buf_ptr < sizeof( uint16_t ) ) {
		return 0;
	}

	uint16_t num_blocks = *reinterpret_cast<const uint16_t*>( &buffer[buf_ptr] );
	buf_ptr += sizeof( num_blocks );

	if( num_blocks == 0 ) {
		throw BogusDataException( "Invalid number of blocks." );
	}

	// Check if all blocks are present.
	if( buffer_size - buf_ptr < sizeof( uint16_t ) * num_blocks ) {
		return 0;
	}

	// Read blocks.
	BlockVector blocks( num_blocks );

	for( std::size_t block_idx = 0; block_idx < num_blocks; ++block_idx ) {
		blocks[block_idx] = *reinterpret_cast<const uint16_t*>( &buffer[buf_ptr] );
		buf_ptr += sizeof( uint16_t );

		// Check for valid ID.
		if( (blocks[block_idx] & 0x0fff) >= num_class_ids ) {
			throw BogusDataException( "Block references invalid class ID." );
		}
	}

	// Everything okay, store class IDs and block data.
	m_class_ids.clear();

	for( std::size_t class_id_idx = 0; class_id_idx < num_class_ids; ++class_id_idx ) {
		m_class_ids.push_back( std::string( class_id_ptrs[class_id_idx].first, class_id_ptrs[class_id_idx].second ) );
	}

	std::swap( m_blocks, blocks );

	return buf_ptr;
}

std::size_t Chunk::get_num_blocks() const {
	return m_blocks.size();
}

void Chunk::add_block( const std::string& class_id, uint8_t flags ) {
	assert( class_id.empty() == false );
	assert( class_id.size() < 256 );
	assert( m_class_ids.size() < 0x0fff );
	assert( m_blocks.size() < 0xffff );

	// Check if class ID is already present.
	ClassIDVector::iterator class_iter = std::find( m_class_ids.begin(), m_class_ids.end(), class_id ); // TODO: Make this faster by using a map? (id -> index mapping)

	if( class_iter == m_class_ids.end() ) {
		// Add!
		m_class_ids.push_back( class_id );
		class_iter = m_class_ids.end() - 1;
	}

	m_blocks.push_back(
		static_cast<uint16_t>(
			static_cast<uint16_t>( (class_iter - m_class_ids.begin()) & 0x0fff ) |
			static_cast<uint16_t>( flags << 12 )
		)
	);
}

const std::string& Chunk::get_block_class_id( std::size_t index ) const {
	assert( index < m_blocks.size() );
	return m_class_ids[m_blocks[index] & 0x0fff];
}

uint8_t Chunk::get_block_flags( std::size_t index ) const {
	assert( index < m_blocks.size() );
	return static_cast<uint8_t>( m_blocks[index] >> 12 );
}

}
}
