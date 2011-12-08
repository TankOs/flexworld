#include <FlexWorld/Class.hpp>
#include <FlexWorld/ClassCache.hpp>
#include <FlexWorld/ResourceId.hpp>
#include <FlexWorld/Chunk.hpp>
#include <FlexWorld/Planet.hpp>
#include <FlexWorld/SHA1.hpp>
#include <FlexWorld/Account.hpp>
#include <FlexWorld/AccountDriver.hpp>
#include <FlexWorld/MessageMeta.hpp>

#include <iostream>

#define BOOST_TEST_MODULE FlexWorld
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE( ResourceId ) {
	BOOST_CHECK_THROW( flex::ResourceId( ".invalid/thing" ), flex::ResourceId::ParserException );
	BOOST_CHECK_THROW( flex::ResourceId( "invalid" ), flex::ResourceId::ParserException );
	BOOST_CHECK_THROW( flex::ResourceId( "invalid/" ), flex::ResourceId::ParserException );
	BOOST_CHECK_THROW( flex::ResourceId( "  " ), flex::ResourceId::ParserException );
	BOOST_CHECK_THROW( flex::ResourceId( "/bleh" ), flex::ResourceId::ParserException );

	flex::ResourceId resource( "fw.weapons/sword" );

	BOOST_CHECK( resource.get_package_id() == "fw.weapons" );
	BOOST_CHECK( resource.get_path() == "sword" );
	BOOST_CHECK( resource.get() == "fw.weapons/sword" );

	flex::ResourceId same( "fw.weapons/sword" );
	BOOST_CHECK( resource == same );

	flex::ResourceId other( "fw.weapons/ball" );
	BOOST_CHECK( resource != other );
}

BOOST_AUTO_TEST_CASE( Class ) {
	flex::Class cls( flex::ResourceId( "fw.base/grass" ) );
	cls.set_name( "Grass" );
	cls.set_origin( sf::Vector3f( 1.f, 2.f, 3.f ) );

	flex::Resource tex0( flex::ResourceId( "fw.base/0.png" ) );
	flex::Resource tex1( flex::ResourceId( "fw.base/1.png" ) );
	flex::Resource tex2( flex::ResourceId( "fw.base/2.png" ) );
	cls.add_texture( tex0 );
	cls.add_texture( tex1 );
	cls.add_texture( tex2 );

	cls.set_hook( "default", sf::Vector3f( 123.f, 123.f, 123.f ) );
	cls.set_hook( "hand", sf::Vector3f( 555.f, 555.f, 555.f ) );

	BOOST_CHECK( cls.get_id().get() == "fw.base/grass" );
	BOOST_CHECK( cls.get_name() == "Grass" );

	BOOST_CHECK( cls.get_texture( 0 ) );
	BOOST_CHECK( cls.get_texture( 1 ) );
	BOOST_CHECK( cls.get_texture( 2 ) );
	BOOST_CHECK( !cls.get_texture( 3 ) );

	BOOST_CHECK( cls.get_texture( 0 )->get_id().get() == "fw.base/0.png" );
	BOOST_CHECK( cls.get_texture( 1 )->get_id().get() == "fw.base/1.png" );
	BOOST_CHECK( cls.get_texture( 2 )->get_id().get() == "fw.base/2.png" );

	BOOST_CHECK( cls.get_hook( "default" ) );
	BOOST_CHECK( cls.get_hook( "hand" ) );
	BOOST_CHECK( !cls.get_hook( "invalid" ) );

	BOOST_CHECK( *cls.get_hook( "default" ) == sf::Vector3f( 123.f, 123.f, 123.f ) );
	BOOST_CHECK( *cls.get_hook( "hand" ) == sf::Vector3f( 555.f, 555.f, 555.f ) );
}

BOOST_AUTO_TEST_CASE( ClassCache ) {
	flex::ClassCache cache;
	flex::Class cls( flex::ResourceId( "fw.base/grass" ) );

	BOOST_CHECK( cache.get_size() == 0 );
	BOOST_CHECK( cache.get_class( 0 ) == nullptr );
	BOOST_CHECK( cache.get_class( 1 ) == nullptr );
	BOOST_CHECK( cache.get_class( 2 ) == nullptr );
	BOOST_CHECK( cache.get_num_holes() == 0 );

	BOOST_CHECK( cache.cache( cls ) == 1 );
	BOOST_CHECK( cache.get_size() == 1 );
	BOOST_CHECK( cache.get_class( 1 ) == &cls );
	BOOST_CHECK( cache.get_class( 2 ) == nullptr );
	BOOST_CHECK( cache.get_use_count( cls ) == 1 );
	BOOST_CHECK( cache.get_num_holes() == 0 );

	BOOST_CHECK( cache.cache( cls ) == 1 );
	BOOST_CHECK( cache.get_size() == 1 );
	BOOST_CHECK( cache.get_class( 1 ) == &cls );
	BOOST_CHECK( cache.get_class( 2 ) == nullptr );
	BOOST_CHECK( cache.get_use_count( cls ) == 2 );
	BOOST_CHECK( cache.get_num_holes() == 0 );

	cache.forget( cls );
	BOOST_CHECK( cache.get_size() == 1 );
	BOOST_CHECK( cache.get_class( 1 ) == &cls );
	BOOST_CHECK( cache.get_class( 2 ) == nullptr );
	BOOST_CHECK( cache.get_use_count( cls ) == 1 );
	BOOST_CHECK( cache.get_num_holes() == 0 );

	cache.forget( cls );
	BOOST_CHECK( cache.get_size() == 0 );
	BOOST_CHECK( cache.get_class( 1 ) == nullptr );
	BOOST_CHECK( cache.get_class( 2 ) == nullptr );
	BOOST_CHECK( cache.get_use_count( cls ) == 0 );
	BOOST_CHECK( cache.get_num_holes() == 0 );

	flex::Class cls0( flex::ResourceId( "fw/class0" ) );
	flex::Class cls1( flex::ResourceId( "fw/class1" ) );

	BOOST_CHECK( cache.cache( cls0 ) == 1 );
	BOOST_CHECK( cache.cache( cls0 ) == 1 );
	BOOST_CHECK( cache.cache( cls1 ) == 2 );
	BOOST_CHECK( cache.cache( cls1 ) == 2 );
	BOOST_CHECK( cache.get_size() == 2 );
	BOOST_CHECK( cache.get_num_holes() == 0 );
	BOOST_CHECK( cache.get_class( 1 ) == &cls0 );
	BOOST_CHECK( cache.get_class( 2 ) == &cls1 );
	BOOST_CHECK( cache.get_use_count( cls0 ) == 2 );
	BOOST_CHECK( cache.get_use_count( cls1 ) == 2 );

	cache.forget( cls0 );
	BOOST_CHECK( cache.get_use_count( cls0 ) == 1 );
	BOOST_CHECK( cache.get_size() == 2 );
	BOOST_CHECK( cache.get_num_holes() == 0 );

	cache.forget( cls0 );
	BOOST_CHECK( cache.get_use_count( cls0 ) == 0 );
	BOOST_CHECK( cache.get_num_holes() == 1 );
	BOOST_CHECK( cache.get_size() == 1 );

	cache.cache( cls0 );
	BOOST_CHECK( cache.get_size() == 2 );
	BOOST_CHECK( cache.get_num_holes() == 0 );
	BOOST_CHECK( cache.get_use_count( cls0 ) == 1 );
	BOOST_CHECK( cache.get_use_count( cls1 ) == 2 );

	//cache.cache_class( cls );
	//BOOST_CHECK( cache.get_class_count() == 1 );
}

BOOST_AUTO_TEST_CASE( FlexChunk ) {
	using namespace flex;

	Chunk chunk( Chunk::Vector( 16, 16, 16 ) );

	BOOST_CHECK( chunk.get_size() == flex::Chunk::Vector( 16, 16, 16 ) );

	chunk.clear();
	BOOST_CHECK( chunk.get_size() == flex::Chunk::Vector( 16, 16, 16 ) );
	BOOST_CHECK( chunk.get_block( Chunk::Vector( 0, 0, 0 ) ) == 0 );

	chunk.set_block( Chunk::Vector( 5, 4, 3 ), 1 );
	chunk.set_block( Chunk::Vector( 5, 2, 3 ), 2 );
	chunk.set_block( Chunk::Vector( 5, 4, 1 ), 3 );

	BOOST_CHECK( chunk.get_block( Chunk::Vector( 5, 4, 3 ) ) == 1 );
	BOOST_CHECK( chunk.get_block( Chunk::Vector( 5, 2, 3 ) ) == 2 );
	BOOST_CHECK( chunk.get_block( Chunk::Vector( 5, 4, 1 ) ) == 3 );
}

BOOST_AUTO_TEST_CASE( FlexPlanet ) {
	using namespace flex;

	Planet::Vector size( 5000, 15, 5000 );
	Chunk::Vector chunk_size( 16, 16, 16 );
	flex::Class cls0( flex::ResourceId( "fw/class0" ) );
	flex::Class cls1( flex::ResourceId( "fw/class1" ) );

	Planet planet( "test", size, chunk_size );

	// Test basic properties.
	BOOST_CHECK( planet.get_id() == "test" );
	BOOST_CHECK( planet.get_size() == size );
	BOOST_CHECK( planet.get_chunk_size() == chunk_size );

	// Test getting non-existing chunk.
	const Chunk* chunk( planet.get_chunk( Planet::Vector( 0, 0, 0 ) ) );
	BOOST_CHECK( chunk == nullptr );
	BOOST_CHECK( planet.get_num_chunks() == 0 );

	// Test coordinate conversion and transformation result.
	Planet::Vector chunk_position( 0, 0, 0 );
	Chunk::Vector block_position( 0, 0, 0 );
	sf::Vector3f coordinate( 132.f, 433.f, 533.f );

	BOOST_CHECK( planet.transform_coordinate( coordinate, chunk_position, block_position ) == false ); // Coord out of scope.
	BOOST_CHECK( chunk_position == Planet::Vector( 8, 27, 33 ) );
	BOOST_CHECK( block_position == Chunk::Vector( 4, 1, 5 ) );

	// Wrong coord.
	BOOST_CHECK( planet.set_block( coordinate, cls0 ) == false );
	BOOST_CHECK( planet.create_chunk( chunk_position ) == false );

	coordinate = sf::Vector3f( 132.f, 33.f, 533.f );
	BOOST_CHECK( planet.transform_coordinate( coordinate, chunk_position, block_position ) );
	BOOST_CHECK( chunk_position == Planet::Vector( 8, 2, 33 ) );
	BOOST_CHECK( block_position == Chunk::Vector( 4, 1, 5 ) );

	// Check planet and class cache are really empty.
	BOOST_CHECK( planet.get_block( coordinate ) == nullptr );
	BOOST_CHECK( planet.get_class_cache().get_size() == 0 );
	BOOST_CHECK( planet.get_class_cache().get_use_count( cls0 ) == 0 );

	BOOST_CHECK( planet.set_block( coordinate, cls0 ) == false ); // No chunk.

	// Create chunk.
	BOOST_CHECK( planet.create_chunk( chunk_position ) );
	BOOST_CHECK( planet.create_chunk( chunk_position ) == false ); // Double.
	BOOST_CHECK( planet.get_chunk( chunk_position ) ); // Verify.

	// Create block and verify internal states.
	BOOST_CHECK( planet.set_block( coordinate, cls0 ) );
	BOOST_CHECK( planet.get_block( coordinate ) == &cls0 );
	BOOST_CHECK( planet.get_class_cache().get_size() == 1 );
	BOOST_CHECK( planet.get_class_cache().get_use_count( cls0 ) == 1 );

	// Same class, different place. No chunk at first.
	sf::Vector3f coordinate2( 152.f, 17.f, 299.f );
	BOOST_CHECK( planet.transform_coordinate( coordinate2, chunk_position, block_position ) );

	BOOST_CHECK( planet.set_block( coordinate2, cls0 ) == false );
	BOOST_CHECK( planet.get_block( coordinate2 ) == nullptr );
	BOOST_CHECK( planet.get_class_cache().get_size() == 1 );
	BOOST_CHECK( planet.get_class_cache().get_use_count( cls0 ) == 1 );

	// Create chunk and set block again.
	BOOST_CHECK( planet.create_chunk( chunk_position ) );
	BOOST_CHECK( planet.create_chunk( chunk_position ) == false );

	BOOST_CHECK( planet.set_block( coordinate2, cls0 ) );
	BOOST_CHECK( planet.get_block( coordinate2 ) == &cls0 );
	BOOST_CHECK( planet.get_class_cache().get_size() == 1 );
	BOOST_CHECK( planet.get_class_cache().get_use_count( cls0 ) == 2 );

	// Overwrite block.
	BOOST_CHECK( planet.set_block( coordinate2, cls1 ) );
	BOOST_CHECK( planet.get_block( coordinate2 ) == &cls1 );
	BOOST_CHECK( planet.get_class_cache().get_size() == 2 );
	BOOST_CHECK( planet.get_class_cache().get_use_count( cls0 ) == 1 );
	BOOST_CHECK( planet.get_class_cache().get_use_count( cls1 ) == 1 );

	// Delete blocks.
	BOOST_CHECK( planet.reset_block( coordinate2 ) );
	BOOST_CHECK( planet.get_block( coordinate2 ) == nullptr );
	BOOST_CHECK( planet.get_class_cache().get_size() == 1 );
	BOOST_CHECK( planet.get_class_cache().get_use_count( cls0 ) == 1 );
	BOOST_CHECK( planet.get_class_cache().get_use_count( cls1 ) == 0 );

	BOOST_CHECK( planet.reset_block( coordinate ) );
	BOOST_CHECK( planet.get_block( coordinate ) == nullptr );
	BOOST_CHECK( planet.get_class_cache().get_size() == 0 );
	BOOST_CHECK( planet.get_class_cache().get_use_count( cls0 ) == 0 );
	BOOST_CHECK( planet.get_class_cache().get_use_count( cls1 ) == 0 );
}

BOOST_AUTO_TEST_CASE( FlexSHA1 ) {
	// Check correct SHA1 calculation.
	BOOST_CHECK( flex::SHA1::MakeHexDigest( "12345" ) == "8cb2237d0679ca88db6464eac60da96345513964" );
	BOOST_CHECK( flex::SHA1::MakeHexDigest( "sdfljsldfjSDFJKSDJ__%$U§NF(%FN(UUJFK" ) == "653e58a35abe7c746a19f906ba91f350f2e57924" );
	BOOST_CHECK( flex::SHA1::MakeHexDigest( "" ) == "da39a3ee5e6b4b0d3255bfef95601890afd80709" );
	BOOST_CHECK( flex::SHA1::MakeHexDigest( "" ) != "random" );
}

BOOST_AUTO_TEST_CASE( FlexAccount ) {
	using namespace flex;

	// Create account, set properties and check.
	Account account;
	account.set_username( "Tank" );
	account.set_password( SHA1::MakeHexDigest( "password" ) );

	BOOST_CHECK( account.get_username() == "Tank" );
	BOOST_CHECK( account.get_password() == SHA1::MakeHexDigest( "password" ) );

	// Test driver.
	// Serialize.
	std::string expected_output = "Username: " + account.get_username() + "\nPassword: " + account.get_password();
	BOOST_CHECK( AccountDriver::serialize( account ) == expected_output );

	// Deserialize.
	Account loaded;
	BOOST_CHECK( AccountDriver::deserialize( expected_output, loaded ) );

	BOOST_CHECK( loaded.get_username() == account.get_username() );
	BOOST_CHECK( loaded.get_password() == account.get_password() );
}

BOOST_AUTO_TEST_CASE( FlexMessageMeta ) {
	using namespace flex;

	// Init state.
	MessageMeta meta;

	BOOST_CHECK( meta.get_field_count() == 0 );
	BOOST_CHECK( meta.get_field_type( 0 ) == MessageMeta::INVALID );
	BOOST_CHECK( meta.get_field_type( 4545 ) == MessageMeta::INVALID );

	// Add fields.
	BOOST_CHECK( meta.add_field( MessageMeta::STRING ) ); // Username.
	BOOST_CHECK( meta.add_field( MessageMeta::STRING ) ); // Password.
	BOOST_CHECK( meta.add_field( MessageMeta::BYTE ) ); // Version.

	BOOST_CHECK( meta.get_field_count() == 3 );
	BOOST_CHECK( meta.get_field_type( 0 ) == MessageMeta::STRING );
	BOOST_CHECK( meta.get_field_type( 1 ) == MessageMeta::STRING );
	BOOST_CHECK( meta.get_field_type( 2 ) == MessageMeta::BYTE );
	BOOST_CHECK( meta.get_field_type( 3 ) == MessageMeta::INVALID );
}
