#include <FlexWorld/ModelDriver.hpp>
#include <FlexWorld/Model.hpp>
#include <FlexWorld/Mesh.hpp>
#include <FlexWorld/Vertex.hpp>
#include <FlexWorld/Triangle.hpp>

#include <boost/test/unit_test.hpp>

bool deserialize_and_check_exception( const flex::ModelDriver::Buffer& buffer, const std::string& expected_what ) {
	bool correct = false;

	try {
		flex::ModelDriver::deserialize( buffer );
	}
	catch( const flex::ModelDriver::DeserializationException& e ) {
		correct = (e.what() == expected_what);
	}

	return correct;
}

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

		BOOST_CHECK_NO_THROW( model = ModelDriver::deserialize( source_buffer ) );

		// Validate.
		BOOST_CHECK( model.get_num_meshes() == 2 );

		// Mesh 0.
		{
			const Mesh& mesh = model.get_mesh( 0 );

			BOOST_CHECK( mesh.get_texture_slot() == 0 );
			BOOST_CHECK( mesh.get_num_vertices() == 4 );
			BOOST_CHECK( mesh.get_num_triangles() == 2 );

			BOOST_CHECK( mesh.get_vertex( 0 ) == Vertex( sf::Vector3f( 0, 1, 0 ), sf::Vector3f( -1, +1, -1 ), sf::Vector2f( 0, 0 ) ) );
			BOOST_CHECK( mesh.get_vertex( 1 ) == Vertex( sf::Vector3f( 0, 1, 1 ), sf::Vector3f( -1, +1, +1 ), sf::Vector2f( 0, 0 ) ) );
			BOOST_CHECK( mesh.get_vertex( 2 ) == Vertex( sf::Vector3f( 1, 1, 1 ), sf::Vector3f( +1, +1, +1 ), sf::Vector2f( 0, 0 ) ) );
			BOOST_CHECK( mesh.get_vertex( 3 ) == Vertex( sf::Vector3f( 1, 1, 0 ), sf::Vector3f( +1, +1, -1 ), sf::Vector2f( 0, 0 ) ) );

			BOOST_CHECK( mesh.get_triangle( 0 ) == Triangle( 0, 1, 2 ) );
			BOOST_CHECK( mesh.get_triangle( 1 ) == Triangle( 1, 2, 3 ) );
		}

		// Mesh 1.
		{
			const Mesh& mesh = model.get_mesh( 1 );

			BOOST_CHECK( mesh.get_texture_slot() == 1 );
			BOOST_CHECK( mesh.get_num_vertices() == 8 );
			BOOST_CHECK( mesh.get_num_triangles() == 10 );

			BOOST_CHECK( mesh.get_vertex( 0 ) == Vertex( sf::Vector3f( 0, 1, 0 ), sf::Vector3f( -1, +1, -1 ), sf::Vector2f( 0, 0 ) ) );
			BOOST_CHECK( mesh.get_vertex( 1 ) == Vertex( sf::Vector3f( 0, 1, 1 ), sf::Vector3f( -1, +1, +1 ), sf::Vector2f( 0, 0 ) ) );
			BOOST_CHECK( mesh.get_vertex( 2 ) == Vertex( sf::Vector3f( 1, 1, 1 ), sf::Vector3f( +1, +1, +1 ), sf::Vector2f( 0, 0 ) ) );
			BOOST_CHECK( mesh.get_vertex( 3 ) == Vertex( sf::Vector3f( 1, 1, 0 ), sf::Vector3f( +1, +1, -1 ), sf::Vector2f( 0, 0 ) ) );
			BOOST_CHECK( mesh.get_vertex( 4 ) == Vertex( sf::Vector3f( 0, 0, 0 ), sf::Vector3f( -1, -1, -1 ), sf::Vector2f( 0, 0 ) ) );
			BOOST_CHECK( mesh.get_vertex( 5 ) == Vertex( sf::Vector3f( 0, 0, 1 ), sf::Vector3f( -1, -1, +1 ), sf::Vector2f( 0, 0 ) ) );
			BOOST_CHECK( mesh.get_vertex( 6 ) == Vertex( sf::Vector3f( 1, 0, 1 ), sf::Vector3f( +1, -1, +1 ), sf::Vector2f( 0, 0 ) ) );
			BOOST_CHECK( mesh.get_vertex( 7 ) == Vertex( sf::Vector3f( 1, 0, 0 ), sf::Vector3f( +1, -1, -1 ), sf::Vector2f( 0, 0 ) ) );

			BOOST_CHECK( mesh.get_triangle( 0 ) == Triangle( 0, 4, 1 ) );
			BOOST_CHECK( mesh.get_triangle( 1 ) == Triangle( 4, 5, 1 ) );
			BOOST_CHECK( mesh.get_triangle( 2 ) == Triangle( 1, 5, 2 ) );
			BOOST_CHECK( mesh.get_triangle( 3 ) == Triangle( 5, 6, 2 ) );
			BOOST_CHECK( mesh.get_triangle( 4 ) == Triangle( 2, 6, 7 ) );
			BOOST_CHECK( mesh.get_triangle( 5 ) == Triangle( 2, 7, 3 ) );
			BOOST_CHECK( mesh.get_triangle( 6 ) == Triangle( 4, 3, 7 ) );
			BOOST_CHECK( mesh.get_triangle( 7 ) == Triangle( 4, 0, 3 ) );
			BOOST_CHECK( mesh.get_triangle( 8 ) == Triangle( 5, 4, 6 ) );
			BOOST_CHECK( mesh.get_triangle( 9 ) == Triangle( 6, 4, 7 ) );
		}
	}

	// Deserialize with wrong data.
	{
		ModelDriver::Buffer buffer;

		// Missing identifier.
		buffer.push_back( 'F' );
		buffer.push_back( 'W' );
		BOOST_CHECK( deserialize_and_check_exception( buffer, "Identifier missing." ) );

		// Wrong identifier.
		buffer.push_back( '_' );
		BOOST_CHECK( deserialize_and_check_exception( buffer, "Wrong identifier." ) );
		buffer[2] = 'M';

		// Version missing.
		BOOST_CHECK( deserialize_and_check_exception( buffer, "Version missing." ) );

		// Wrong version.
		buffer.push_back( static_cast<char>( 0xff ) );
		BOOST_CHECK( deserialize_and_check_exception( buffer, "Wrong version." ) );
		buffer[buffer.size() - 1] = 0x00;

		// Missing number of meshes.
		BOOST_CHECK( deserialize_and_check_exception( buffer, "Mesh count missing." ) );

		// Invalid number of meshes.
		buffer.push_back( 0 );
		BOOST_CHECK( deserialize_and_check_exception( buffer, "Meshes missing." ) );
		buffer[buffer.size() - 1] = 0x01;

		// Missing number of vertices.
		BOOST_CHECK( deserialize_and_check_exception( buffer, "Vertex count missing." ) );

		// Invalid number of vertices.
		buffer.push_back( 0x00 );
		buffer.push_back( 0x00 );
		BOOST_CHECK( deserialize_and_check_exception( buffer, "Vertices missing." ) );

		buffer[buffer.size() - 2] = 0x01;
		buffer[buffer.size() - 1] = 0x00;
		BOOST_CHECK( deserialize_and_check_exception( buffer, "Vertices missing." ) );

		buffer[buffer.size() - 2] = 0x02;
		buffer[buffer.size() - 1] = 0x00;
		BOOST_CHECK( deserialize_and_check_exception( buffer, "Vertices missing." ) );

		buffer[buffer.size() - 2] = 0x03;
		buffer[buffer.size() - 1] = 0x00;

		// Missing number of triangles.
		BOOST_CHECK( deserialize_and_check_exception( buffer, "Triangle count missing." ) );

		// Invalid number of triangles.
		buffer.push_back( 0x00 );
		buffer.push_back( 0x00 );
		BOOST_CHECK( deserialize_and_check_exception( buffer, "Triangles missing." ) );

		buffer[buffer.size() - 2] = 0x01;
		buffer[buffer.size() - 1] = 0x00;

		// Texture slot missing.
		BOOST_CHECK( deserialize_and_check_exception( buffer, "Texture slot missing." ) );
		buffer.push_back( 0x00 );

		// Too less vertices.
		BOOST_CHECK( deserialize_and_check_exception( buffer, "Too less vertices." ) );

		buffer.insert( buffer.end(), sizeof( Vertex ), 0 );
		buffer.insert( buffer.end(), sizeof( Vertex ), 0 );
		buffer.insert( buffer.end(), sizeof( Vertex ), 0 );

		// Too less triangles.
		BOOST_CHECK( deserialize_and_check_exception( buffer, "Too less triangles." ) );

		// Invalid triangles.
		buffer.insert( buffer.end(), sizeof( Triangle ), 0 );
		BOOST_CHECK( deserialize_and_check_exception( buffer, "Invalid triangle." ) );

		buffer[buffer.size() - 6] = 0x01;
		buffer[buffer.size() - 5] = 0x00;
		buffer[buffer.size() - 4] = 0x00;
		buffer[buffer.size() - 3] = 0x00;
		buffer[buffer.size() - 2] = 0x00;
		buffer[buffer.size() - 1] = 0x00;
		BOOST_CHECK( deserialize_and_check_exception( buffer, "Invalid triangle." ) );

		buffer[buffer.size() - 6] = 0x00;
		buffer[buffer.size() - 5] = 0x00;
		buffer[buffer.size() - 4] = 0x01;
		buffer[buffer.size() - 3] = 0x00;
		buffer[buffer.size() - 2] = 0x00;
		buffer[buffer.size() - 1] = 0x00;
		BOOST_CHECK( deserialize_and_check_exception( buffer, "Invalid triangle." ) );

		buffer[buffer.size() - 6] = 0x00;
		buffer[buffer.size() - 5] = 0x00;
		buffer[buffer.size() - 4] = 0x00;
		buffer[buffer.size() - 3] = 0x00;
		buffer[buffer.size() - 2] = 0x01;
		buffer[buffer.size() - 1] = 0x00;
		BOOST_CHECK( deserialize_and_check_exception( buffer, "Invalid triangle." ) );

		// Too high indices.
		buffer[buffer.size() - 6] = 0x00;
		buffer[buffer.size() - 5] = 0x00;
		buffer[buffer.size() - 4] = 0x01;
		buffer[buffer.size() - 3] = 0x00;
		buffer[buffer.size() - 2] = 0x03;
		buffer[buffer.size() - 1] = 0x00;
		BOOST_CHECK( deserialize_and_check_exception( buffer, "Invalid triangle." ) );

		buffer[buffer.size() - 6] = 0x00;
		buffer[buffer.size() - 5] = 0x00;
		buffer[buffer.size() - 4] = 0x03;
		buffer[buffer.size() - 3] = 0x00;
		buffer[buffer.size() - 2] = 0x01;
		buffer[buffer.size() - 1] = 0x00;
		BOOST_CHECK( deserialize_and_check_exception( buffer, "Invalid triangle." ) );

		buffer[buffer.size() - 6] = 0x03;
		buffer[buffer.size() - 5] = 0x00;
		buffer[buffer.size() - 4] = 0x01;
		buffer[buffer.size() - 3] = 0x00;
		buffer[buffer.size() - 2] = 0x00;
		buffer[buffer.size() - 1] = 0x00;
		BOOST_CHECK( deserialize_and_check_exception( buffer, "Invalid triangle." ) );

		// Correct all failures and check.
		buffer[buffer.size() - 6] = 0x00;
		buffer[buffer.size() - 5] = 0x00;
		buffer[buffer.size() - 4] = 0x01;
		buffer[buffer.size() - 3] = 0x00;
		buffer[buffer.size() - 2] = 0x02;
		buffer[buffer.size() - 1] = 0x00;

		BOOST_CHECK_NO_THROW( ModelDriver::deserialize( buffer ) );
	}

}
