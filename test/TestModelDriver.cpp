#include <FlexWorld/ModelDriver.hpp>
#include <FlexWorld/Model.hpp>
#include <FlexWorld/Mesh.hpp>
#include <FlexWorld/Triangle.hpp>

#include <FWSG/Vertex.hpp>
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

	float value = 0;
	FloatCuboid cuboid;
	sf::FloatRect rect;

	ModelDriver::Buffer source_buffer;
	source_buffer.push_back( 'F' );
	source_buffer.push_back( 'W' );
	source_buffer.push_back( 'M' );
	source_buffer.push_back( 0x00 ); // Version.

	// Bounding box (x, y, z, width, height, depth).
	cuboid = FloatCuboid( 0, 2, 4, 10, 20, 30 );
	source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &cuboid ), reinterpret_cast<const char*>( &cuboid ) + sizeof( cuboid ) );

	// Coverage rects.
	rect = sf::FloatRect( 100, 101, 102, 103 );
	source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &rect ), reinterpret_cast<const char*>( &rect ) + sizeof( rect ) );
	rect = sf::FloatRect( 200, 201, 202, 203 );
	source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &rect ), reinterpret_cast<const char*>( &rect ) + sizeof( rect ) );
	rect = sf::FloatRect( 300, 301, 302, 303 );
	source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &rect ), reinterpret_cast<const char*>( &rect ) + sizeof( rect ) );
	rect = sf::FloatRect( 400, 401, 402, 403 );
	source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &rect ), reinterpret_cast<const char*>( &rect ) + sizeof( rect ) );
	rect = sf::FloatRect( 500, 501, 502, 503 );
	source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &rect ), reinterpret_cast<const char*>( &rect ) + sizeof( rect ) );
	rect = sf::FloatRect( 600, 601, 602, 603 );
	source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &rect ), reinterpret_cast<const char*>( &rect ) + sizeof( rect ) );

	// Block scale divisor.
	value = 33.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );

	source_buffer.push_back( 0x02 ); // Num meshes.

	// Mesh 0.
	source_buffer.push_back( 0x04 ); // Num vertices.
	source_buffer.push_back( 0x00 );
	source_buffer.push_back( 0x02 ); // Num triangles.
	source_buffer.push_back( 0x00 );
	source_buffer.push_back( 0x00 ); // Texture slot.

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
	value = +1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );

	value = +1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );

	value = +1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +0.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = -1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
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
	value = +1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );

	value = +1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );

	value = +1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +0.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = -1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
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
	value = +1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );

	value = +1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +0.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = -1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );

	value = +1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +0.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +0.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = -1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = -1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
	value = +1.0f; source_buffer.insert( source_buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
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

		model.set_bounding_box( flex::FloatCuboid( 0, 2, 4, 10, 20, 30 ) );
		model.set_face_coverage( flex::UP_FACE, sf::FloatRect( 100, 101, 102, 103 ) );
		model.set_face_coverage( flex::DOWN_FACE, sf::FloatRect( 200, 201, 202, 203 ) );
		model.set_face_coverage( flex::BACK_FACE, sf::FloatRect( 300, 301, 302, 303 ) );
		model.set_face_coverage( flex::RIGHT_FACE, sf::FloatRect( 400, 401, 402, 403 ) );
		model.set_face_coverage( flex::FRONT_FACE, sf::FloatRect( 500, 501, 502, 503 ) );
		model.set_face_coverage( flex::LEFT_FACE, sf::FloatRect( 600, 601, 602, 603 ) );
		model.set_block_scale_divisor( 33.0f );

		// Top vertices first.
		Mesh mesh;

		mesh.set_texture_slot( 0 );

		mesh.add_vertex( sg::Vertex( sf::Vector3f( 0, 1, 0 ), sf::Vector3f( -1, +1, -1 ), sf::Vector2f( 0, 0 ) ) ); // Bac left.
		mesh.add_vertex( sg::Vertex( sf::Vector3f( 0, 1, 1 ), sf::Vector3f( -1, +1, +1 ), sf::Vector2f( 0, 1 ) ) ); // Fwd left.
		mesh.add_vertex( sg::Vertex( sf::Vector3f( 1, 1, 1 ), sf::Vector3f( +1, +1, +1 ), sf::Vector2f( 1, 1 ) ) ); // Fwd right.
		mesh.add_vertex( sg::Vertex( sf::Vector3f( 1, 1, 0 ), sf::Vector3f( +1, +1, -1 ), sf::Vector2f( 1, 0 ) ) ); // Bac right.

		mesh.define_triangle( Triangle( 0, 1, 2 ) );
		mesh.define_triangle( Triangle( 1, 2, 3 ) );

		model.add_mesh( mesh );

		// Rest.
		mesh.clear();

		mesh.set_texture_slot( 1 );

		mesh.add_vertex( sg::Vertex( sf::Vector3f( 0, 1, 0 ), sf::Vector3f( -1, +1, -1 ), sf::Vector2f( 0, 0 ) ) ); // Top bac left.
		mesh.add_vertex( sg::Vertex( sf::Vector3f( 0, 1, 1 ), sf::Vector3f( -1, +1, +1 ), sf::Vector2f( 0, 1 ) ) ); // Top fwd left.
		mesh.add_vertex( sg::Vertex( sf::Vector3f( 1, 1, 1 ), sf::Vector3f( +1, +1, +1 ), sf::Vector2f( 1, 1 ) ) ); // Top fwd right.
		mesh.add_vertex( sg::Vertex( sf::Vector3f( 1, 1, 0 ), sf::Vector3f( +1, +1, -1 ), sf::Vector2f( 1, 0 ) ) ); // Top bac right.

		mesh.add_vertex( sg::Vertex( sf::Vector3f( 0, 0, 0 ), sf::Vector3f( -1, -1, -1 ), sf::Vector2f( 0, 0 ) ) ); // Bot bac left.
		mesh.add_vertex( sg::Vertex( sf::Vector3f( 0, 0, 1 ), sf::Vector3f( -1, -1, +1 ), sf::Vector2f( 0, 1 ) ) ); // Bot fwd left.
		mesh.add_vertex( sg::Vertex( sf::Vector3f( 1, 0, 1 ), sf::Vector3f( +1, -1, +1 ), sf::Vector2f( 1, 1 ) ) ); // Bot fwd right.
		mesh.add_vertex( sg::Vertex( sf::Vector3f( 1, 0, 0 ), sf::Vector3f( +1, -1, -1 ), sf::Vector2f( 1, 0 ) ) ); // Bot bac right.

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
		BOOST_CHECK( model.get_bounding_box() == flex::FloatCuboid( 0, 2, 4, 10, 20, 30 ) );
		BOOST_CHECK( model.get_face_coverage( flex::UP_FACE ) == sf::FloatRect( 100, 101, 102, 103 ) );
		BOOST_CHECK( model.get_face_coverage( flex::DOWN_FACE ) == sf::FloatRect( 200, 201, 202, 203 ) );
		BOOST_CHECK( model.get_face_coverage( flex::BACK_FACE ) == sf::FloatRect( 300, 301, 302, 303 ) );
		BOOST_CHECK( model.get_face_coverage( flex::RIGHT_FACE ) == sf::FloatRect( 400, 401, 402, 403 ) );
		BOOST_CHECK( model.get_face_coverage( flex::FRONT_FACE ) == sf::FloatRect( 500, 501, 502, 503 ) );
		BOOST_CHECK( model.get_face_coverage( flex::LEFT_FACE ) == sf::FloatRect( 600, 601, 602, 603 ) );
		BOOST_CHECK( model.get_block_scale_divisor() == 33.0f );
		BOOST_CHECK( model.get_num_meshes() == 2 );

		// Mesh 0.
		{
			const Mesh& mesh = model.get_mesh( 0 );

			BOOST_CHECK( mesh.get_texture_slot() == 0 );
			BOOST_CHECK( mesh.get_num_vertices() == 4 );
			BOOST_CHECK( mesh.get_num_triangles() == 2 );

			BOOST_CHECK( mesh.get_vertex( 0 ) == sg::Vertex( sf::Vector3f( 0, 1, 0 ), sf::Vector3f( -1, +1, -1 ), sf::Vector2f( 0, 0 ) ) );
			BOOST_CHECK( mesh.get_vertex( 1 ) == sg::Vertex( sf::Vector3f( 0, 1, 1 ), sf::Vector3f( -1, +1, +1 ), sf::Vector2f( 0, 1 ) ) );
			BOOST_CHECK( mesh.get_vertex( 2 ) == sg::Vertex( sf::Vector3f( 1, 1, 1 ), sf::Vector3f( +1, +1, +1 ), sf::Vector2f( 1, 1 ) ) );
			BOOST_CHECK( mesh.get_vertex( 3 ) == sg::Vertex( sf::Vector3f( 1, 1, 0 ), sf::Vector3f( +1, +1, -1 ), sf::Vector2f( 1, 0 ) ) );

			BOOST_CHECK( mesh.get_triangle( 0 ) == Triangle( 0, 1, 2 ) );
			BOOST_CHECK( mesh.get_triangle( 1 ) == Triangle( 1, 2, 3 ) );
		}

		// Mesh 1.
		{
			const Mesh& mesh = model.get_mesh( 1 );

			BOOST_CHECK( mesh.get_texture_slot() == 1 );
			BOOST_CHECK( mesh.get_num_vertices() == 8 );
			BOOST_CHECK( mesh.get_num_triangles() == 10 );

			BOOST_CHECK( mesh.get_vertex( 0 ) == sg::Vertex( sf::Vector3f( 0, 1, 0 ), sf::Vector3f( -1, +1, -1 ), sf::Vector2f( 0, 0 ) ) );
			BOOST_CHECK( mesh.get_vertex( 1 ) == sg::Vertex( sf::Vector3f( 0, 1, 1 ), sf::Vector3f( -1, +1, +1 ), sf::Vector2f( 0, 1 ) ) );
			BOOST_CHECK( mesh.get_vertex( 2 ) == sg::Vertex( sf::Vector3f( 1, 1, 1 ), sf::Vector3f( +1, +1, +1 ), sf::Vector2f( 1, 1 ) ) );
			BOOST_CHECK( mesh.get_vertex( 3 ) == sg::Vertex( sf::Vector3f( 1, 1, 0 ), sf::Vector3f( +1, +1, -1 ), sf::Vector2f( 1, 0 ) ) );
			BOOST_CHECK( mesh.get_vertex( 4 ) == sg::Vertex( sf::Vector3f( 0, 0, 0 ), sf::Vector3f( -1, -1, -1 ), sf::Vector2f( 0, 0 ) ) );
			BOOST_CHECK( mesh.get_vertex( 5 ) == sg::Vertex( sf::Vector3f( 0, 0, 1 ), sf::Vector3f( -1, -1, +1 ), sf::Vector2f( 0, 1 ) ) );
			BOOST_CHECK( mesh.get_vertex( 6 ) == sg::Vertex( sf::Vector3f( 1, 0, 1 ), sf::Vector3f( +1, -1, +1 ), sf::Vector2f( 1, 1 ) ) );
			BOOST_CHECK( mesh.get_vertex( 7 ) == sg::Vertex( sf::Vector3f( 1, 0, 0 ), sf::Vector3f( +1, -1, -1 ), sf::Vector2f( 1, 0 ) ) );

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

		// Bounding box missing.
		BOOST_CHECK( deserialize_and_check_exception( buffer, "Bounding box missing." ) );

		value = 0.0f; buffer.insert( buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
		BOOST_CHECK( deserialize_and_check_exception( buffer, "Bounding box missing." ) );

		value = 2.0f; buffer.insert( buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
		BOOST_CHECK( deserialize_and_check_exception( buffer, "Bounding box missing." ) );

		value = 4.0f; buffer.insert( buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
		BOOST_CHECK( deserialize_and_check_exception( buffer, "Bounding box missing." ) );

		value = 10.0f; buffer.insert( buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
		BOOST_CHECK( deserialize_and_check_exception( buffer, "Bounding box missing." ) );

		value = 20.0f; buffer.insert( buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
		BOOST_CHECK( deserialize_and_check_exception( buffer, "Bounding box missing." ) );

		value = 30.0f; buffer.insert( buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );

		// Wrong bounding box.
		*reinterpret_cast<float*>( &buffer[buffer.size() - 6 * sizeof( value )] ) = -1.0f;
		BOOST_CHECK( deserialize_and_check_exception( buffer, "Wrong bounding box." ) );
		*reinterpret_cast<float*>( &buffer[buffer.size() - 6 * sizeof( value )] ) = 0.0f;

		*reinterpret_cast<float*>( &buffer[buffer.size() - 5 * sizeof( value )] ) = -1.0f;
		BOOST_CHECK( deserialize_and_check_exception( buffer, "Wrong bounding box." ) );
		*reinterpret_cast<float*>( &buffer[buffer.size() - 5 * sizeof( value )] ) = 2.0f;

		*reinterpret_cast<float*>( &buffer[buffer.size() - 4 * sizeof( value )] ) = -1.0f;
		BOOST_CHECK( deserialize_and_check_exception( buffer, "Wrong bounding box." ) );
		*reinterpret_cast<float*>( &buffer[buffer.size() - 4 * sizeof( value )] ) = 3.0f;

		*reinterpret_cast<float*>( &buffer[buffer.size() - 3 * sizeof( value )] ) = 0.0f;
		BOOST_CHECK( deserialize_and_check_exception( buffer, "Wrong bounding box." ) );
		*reinterpret_cast<float*>( &buffer[buffer.size() - 3 * sizeof( value )] ) = 10.0f;

		*reinterpret_cast<float*>( &buffer[buffer.size() - 2 * sizeof( value )] ) = 0.0f;
		BOOST_CHECK( deserialize_and_check_exception( buffer, "Wrong bounding box." ) );
		*reinterpret_cast<float*>( &buffer[buffer.size() - 2 * sizeof( value )] ) = 20.0f;

		*reinterpret_cast<float*>( &buffer[buffer.size() - 1 * sizeof( value )] ) = 0.0f;
		BOOST_CHECK( deserialize_and_check_exception( buffer, "Wrong bounding box." ) );
		*reinterpret_cast<float*>( &buffer[buffer.size() - 1 * sizeof( value )] ) = 30.0f;

		// Coverage rects missing.
		rect = sf::FloatRect( 1, 2, 3, 4 );

		for( int face_idx = 0; face_idx < NUM_FACES; ++face_idx ) {
			buffer.insert( buffer.end(), reinterpret_cast<const char*>( &rect ), reinterpret_cast<const char*>( &rect ) + sizeof( rect ) );

			if( face_idx + 1 < NUM_FACES ) {
				BOOST_CHECK( deserialize_and_check_exception( buffer, "Face coverage rect missing." ) );
			}
		}

		// Wrong coverage rects.
		for( int face_idx = 0; face_idx < NUM_FACES; ++face_idx ) {
			*reinterpret_cast<sf::FloatRect*>( &buffer[buffer.size() - (face_idx + 1) * sizeof( sf::FloatRect )] ) = sf::FloatRect( -1, +1, +1 ,+1 );
			BOOST_CHECK( deserialize_and_check_exception( buffer, "Wrong face coverage rect." ) );
			*reinterpret_cast<sf::FloatRect*>( &buffer[buffer.size() - (face_idx + 1) * sizeof( sf::FloatRect )] ) = sf::FloatRect( +1, -1, +1 ,+1 );
			BOOST_CHECK( deserialize_and_check_exception( buffer, "Wrong face coverage rect." ) );
			*reinterpret_cast<sf::FloatRect*>( &buffer[buffer.size() - (face_idx + 1) * sizeof( sf::FloatRect )] ) = sf::FloatRect( +1, +1, -1 ,+1 );
			BOOST_CHECK( deserialize_and_check_exception( buffer, "Wrong face coverage rect." ) );
			*reinterpret_cast<sf::FloatRect*>( &buffer[buffer.size() - (face_idx + 1) * sizeof( sf::FloatRect )] ) = sf::FloatRect( +1, +1, +1 ,-1 );
			BOOST_CHECK( deserialize_and_check_exception( buffer, "Wrong face coverage rect." ) );

			*reinterpret_cast<sf::FloatRect*>( &buffer[buffer.size() - (face_idx + 1) * sizeof( sf::FloatRect )] ) = sf::FloatRect( +1, +1, +1 ,+1 );
		}

		// Block scale divisor missing.
		BOOST_CHECK( deserialize_and_check_exception( buffer, "Block scale divisor missing." ) );

		// Wrong block scale divisor.
		value = 0.9f;
		buffer.insert( buffer.end(), reinterpret_cast<const char*>( &value ), reinterpret_cast<const char*>( &value ) + sizeof( value ) );
		BOOST_CHECK( deserialize_and_check_exception( buffer, "Wrong block scale divisor." ) );
		*reinterpret_cast<float*>( &buffer[buffer.size() - sizeof( value )] ) = 33.0f;

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

		buffer.insert( buffer.end(), sizeof( sg::Vertex ), 0 );
		buffer.insert( buffer.end(), sizeof( sg::Vertex ), 0 );
		buffer.insert( buffer.end(), sizeof( sg::Vertex ), 0 );

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
