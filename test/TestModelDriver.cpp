#include <FlexWorld/ModelDriver.hpp>
#include <FlexWorld/Model.hpp>
#include <FlexWorld/Mesh.hpp>
#include <FlexWorld/Vertex.hpp>
#include <FlexWorld/Triangle.hpp>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE( TestModelDriver ) {
	using namespace flex;

	ModelDriver::Buffer source_buffer;
	source_buffer.push_back( 'F' );
	source_buffer.push_back( 'W' );
	source_buffer.push_back( 'M' );
	source_buffer.push_back( 0x00 ); // Version.
	source_buffer.push_back( 0x02 ); // Num meshes.

	// Mesh 0.
	source_buffer.push_back( 0x04 ); // Num vertices.
	source_buffer.push_back( 0x00 );
	source_buffer.push_back( 0x02 ); // Num triangles.
	source_buffer.push_back( 0x00 );
	source_buffer.push_back( 0x00 ); // Texture slot.

	// Vertices.
	float value = 0;

	value = +0.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +0.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = -1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = -1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +0.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +0.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );

	value = +0.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = -1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +0.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +0.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );

	value = +1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +0.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +0.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );

	value = +1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +0.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = -1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +0.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +0.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );

	Mesh::TriangleIndex index_value = 0;

	// Triangles.
	index_value = 0; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &index_value ), reinterpret_cast<const char*>( &index_value ) + sizeof( index_value ) );
	index_value = 1; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &index_value ), reinterpret_cast<const char*>( &index_value ) + sizeof( index_value ) );
	index_value = 2; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &index_value ), reinterpret_cast<const char*>( &index_value ) + sizeof( index_value ) );

	index_value = 1; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &index_value ), reinterpret_cast<const char*>( &index_value ) + sizeof( index_value ) );
	index_value = 2; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &index_value ), reinterpret_cast<const char*>( &index_value ) + sizeof( index_value ) );
	index_value = 3; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &index_value ), reinterpret_cast<const char*>( &index_value ) + sizeof( index_value ) );

	// Mesh 1.
	source_buffer.push_back( 0x08 ); // Num vertices.
	source_buffer.push_back( 0x00 );
	source_buffer.push_back( 0x0a ); // Num triangles.
	source_buffer.push_back( 0x00 );
	source_buffer.push_back( 0x01 ); // Texture slot.

	// Vertices.
	value = +0.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +0.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = -1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = -1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +0.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +0.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );

	value = +0.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = -1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +0.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +0.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );

	value = +1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +0.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +0.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );

	value = +1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +0.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = -1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +0.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +0.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );

	value = +0.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +0.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +0.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = -1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = -1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = -1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +0.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +0.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );

	value = +0.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +0.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = -1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = -1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +0.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +0.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );

	value = +1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +0.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = -1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +0.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +0.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );

	value = +1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +0.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +0.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = -1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = -1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +0.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +0.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );

	// Triangles.
	index_value = 0; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &index_value ), reinterpret_cast<const char*>( &index_value ) + sizeof( index_value ) );
	index_value = 4; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &index_value ), reinterpret_cast<const char*>( &index_value ) + sizeof( index_value ) );
	index_value = 1; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &index_value ), reinterpret_cast<const char*>( &index_value ) + sizeof( index_value ) );

	index_value = 4; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &index_value ), reinterpret_cast<const char*>( &index_value ) + sizeof( index_value ) );
	index_value = 5; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &index_value ), reinterpret_cast<const char*>( &index_value ) + sizeof( index_value ) );
	index_value = 1; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &index_value ), reinterpret_cast<const char*>( &index_value ) + sizeof( index_value ) );

	index_value = 1; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &index_value ), reinterpret_cast<const char*>( &index_value ) + sizeof( index_value ) );
	index_value = 5; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &index_value ), reinterpret_cast<const char*>( &index_value ) + sizeof( index_value ) );
	index_value = 2; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &index_value ), reinterpret_cast<const char*>( &index_value ) + sizeof( index_value ) );

	index_value = 5; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &index_value ), reinterpret_cast<const char*>( &index_value ) + sizeof( index_value ) );
	index_value = 6; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &index_value ), reinterpret_cast<const char*>( &index_value ) + sizeof( index_value ) );
	index_value = 2; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &index_value ), reinterpret_cast<const char*>( &index_value ) + sizeof( index_value ) );

	index_value = 2; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &index_value ), reinterpret_cast<const char*>( &index_value ) + sizeof( index_value ) );
	index_value = 6; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &index_value ), reinterpret_cast<const char*>( &index_value ) + sizeof( index_value ) );
	index_value = 7; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &index_value ), reinterpret_cast<const char*>( &index_value ) + sizeof( index_value ) );

	index_value = 2; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &index_value ), reinterpret_cast<const char*>( &index_value ) + sizeof( index_value ) );
	index_value = 7; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &index_value ), reinterpret_cast<const char*>( &index_value ) + sizeof( index_value ) );
	index_value = 3; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &index_value ), reinterpret_cast<const char*>( &index_value ) + sizeof( index_value ) );

	index_value = 4; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &index_value ), reinterpret_cast<const char*>( &index_value ) + sizeof( index_value ) );
	index_value = 3; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &index_value ), reinterpret_cast<const char*>( &index_value ) + sizeof( index_value ) );
	index_value = 7; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &index_value ), reinterpret_cast<const char*>( &index_value ) + sizeof( index_value ) );

	index_value = 4; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &index_value ), reinterpret_cast<const char*>( &index_value ) + sizeof( index_value ) );
	index_value = 0; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &index_value ), reinterpret_cast<const char*>( &index_value ) + sizeof( index_value ) );
	index_value = 3; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &index_value ), reinterpret_cast<const char*>( &index_value ) + sizeof( index_value ) );

	index_value = 5; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &index_value ), reinterpret_cast<const char*>( &index_value ) + sizeof( index_value ) );
	index_value = 4; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &index_value ), reinterpret_cast<const char*>( &index_value ) + sizeof( index_value ) );
	index_value = 6; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &index_value ), reinterpret_cast<const char*>( &index_value ) + sizeof( index_value ) );

	index_value = 6; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &index_value ), reinterpret_cast<const char*>( &index_value ) + sizeof( index_value ) );
	index_value = 4; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &index_value ), reinterpret_cast<const char*>( &index_value ) + sizeof( index_value ) );
	index_value = 7; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &index_value ), reinterpret_cast<const char*>( &index_value ) + sizeof( index_value ) );

	// Serialize a cube with a separate top face.
	{
		Model model;

		// Top vertices first.
		Mesh mesh;

		mesh.set_texture_slot( 0 );

		mesh.add_vertex( Vertex( sf::Vector3f( 0, 1, 0 ), sf::Vector3f( -1, +1, -1 ), sf::Vector2f( 0, 0 ) ) ); // Bac left.
		mesh.add_vertex( Vertex( sf::Vector3f( 0, 1, 1 ), sf::Vector3f( -1, +1, +1 ), sf::Vector2f( 0, 0 ) ) ); // Fwd left.
		mesh.add_vertex( Vertex( sf::Vector3f( 1, 1, 1 ), sf::Vector3f( +1, +1, +1 ), sf::Vector2f( 0, 0 ) ) ); // Fwd right.
		mesh.add_vertex( Vertex( sf::Vector3f( 1, 1, 0 ), sf::Vector3f( +1, +1, -1 ), sf::Vector2f( 0, 0 ) ) ); // Bac right.

		mesh.define_triangle( Triangle( 0, 1, 2 ) );
		mesh.define_triangle( Triangle( 1, 2, 3 ) );

		model.add_mesh( mesh );

		// Rest.
		mesh.clear();

		mesh.set_texture_slot( 1 );

		mesh.add_vertex( Vertex( sf::Vector3f( 0, 1, 0 ), sf::Vector3f( -1, +1, -1 ), sf::Vector2f( 0, 0 ) ) ); // Top bac left.
		mesh.add_vertex( Vertex( sf::Vector3f( 0, 1, 1 ), sf::Vector3f( -1, +1, +1 ), sf::Vector2f( 0, 0 ) ) ); // Top fwd left.
		mesh.add_vertex( Vertex( sf::Vector3f( 1, 1, 1 ), sf::Vector3f( +1, +1, +1 ), sf::Vector2f( 0, 0 ) ) ); // Top fwd right.
		mesh.add_vertex( Vertex( sf::Vector3f( 1, 1, 0 ), sf::Vector3f( +1, +1, -1 ), sf::Vector2f( 0, 0 ) ) ); // Top bac right.

		mesh.add_vertex( Vertex( sf::Vector3f( 0, 0, 0 ), sf::Vector3f( -1, -1, -1 ), sf::Vector2f( 0, 0 ) ) ); // Bot bac left.
		mesh.add_vertex( Vertex( sf::Vector3f( 0, 0, 1 ), sf::Vector3f( -1, -1, +1 ), sf::Vector2f( 0, 0 ) ) ); // Bot fwd left.
		mesh.add_vertex( Vertex( sf::Vector3f( 1, 0, 1 ), sf::Vector3f( +1, -1, +1 ), sf::Vector2f( 0, 0 ) ) ); // Bot fwd right.
		mesh.add_vertex( Vertex( sf::Vector3f( 1, 0, 0 ), sf::Vector3f( +1, -1, -1 ), sf::Vector2f( 0, 0 ) ) ); // Bot bac right.

		mesh.define_triangle( Triangle( 0, 4, 1 ) );
		mesh.define_triangle( Triangle( 4, 5, 1 ) );
		mesh.define_triangle( Triangle( 1, 5, 2 ) );
		mesh.define_triangle( Triangle( 5, 6, 2 ) );
		mesh.define_triangle( Triangle( 2, 6, 7 ) );
		mesh.define_triangle( Triangle( 2, 7, 3 ) );
		mesh.define_triangle( Triangle( 4, 3, 7 ) );
		mesh.define_triangle( Triangle( 4, 0, 3 ) );
		mesh.define_triangle( Triangle( 5, 4, 6 ) );
		mesh.define_triangle( Triangle( 6, 4, 7 ) );

		model.add_mesh( mesh );

		BOOST_CHECK( model.get_num_meshes() == 2 );

		ModelDriver::Buffer buffer = ModelDriver::serialize( model );
		BOOST_CHECK( buffer == source_buffer );
	}

	// Deserialize.
	{
		Model model;

		//BOOST_CHECK_NO_THROW( model = ModelDriver::deserialize( source_buffer ) );
	}
}
