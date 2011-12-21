//#include <FlexWorld/Class.hpp>
//#include <FlexWorld/ClassCache.hpp>
#include <FlexWorld/FlexID.hpp>
//#include <FlexWorld/Chunk.hpp>
//#include <FlexWorld/Planet.hpp>
//#include <FlexWorld/SHA1.hpp>
//#include <FlexWorld/Account.hpp>
//#include <FlexWorld/AccountDriver.hpp>
//#include <FlexWorld/MessageMeta.hpp>
//#include <FlexWorld/Message.hpp>

#include <iostream>

#define BOOST_TEST_MODULE FlexWorld
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE( TestFlexID ) {
	using namespace flex;

	// Create FlexID and check initial state.
	{
		FlexID id;

		BOOST_CHECK( id.get() == "" );
		BOOST_CHECK( id.get_package() == "" );
		BOOST_CHECK( id.get_resource() == "" );
		BOOST_CHECK( id.is_valid_package() == false );
		BOOST_CHECK( id.is_valid_resource() == false );
	}

	// Check package character range.
	{
		const std::string valid = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

		for( short code = 0; code < 256; ++code ) {
			char ch = static_cast<char>( code );
			bool should_be_valid = valid.find( ch ) != std::string::npos;
			std::string package;

			package += ch;

			FlexID id;
			BOOST_CHECK( id.set_package( package ) == should_be_valid );
		}

		// Some special cases.
		FlexID id;
		BOOST_CHECK( id.set_package( ".foo" ) == false );
		BOOST_CHECK( id.set_package( "foo." ) == false );
		BOOST_CHECK( id.set_package( "" ) == false );
	}

	// Check setting package.
	{
		FlexID id;

		BOOST_CHECK( id.set_package( "fw.weapons" ) );
		BOOST_CHECK( id.get_package() == "fw.weapons" );
	}

	// Check setting invalid package recovers previous package.
	{
		FlexID id;

		id.set_package( "fw.weapons" );
		id.set_package( "" );

		BOOST_CHECK( id.get_package() == "fw.weapons" );
	}

	// Check setting resource without package.
	{
		FlexID id;
		BOOST_CHECK( id.set_resource( "sword.png" ) == false );
	}

	// Check resource character range.
	{
		const std::string valid = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-.";
		FlexID id;

		// Set package, won't work without.
		id.set_package( "fw.weapons" );

		for( short code = 0; code < 256; ++code ) {
			char ch( static_cast<char>( code ) );
			std::string resource;
			bool should_be_valid = valid.find( ch ) != std::string::npos;

			resource += ch;

			BOOST_CHECK( id.set_resource( resource ) == should_be_valid );
		}

		// Special case.
		BOOST_CHECK( id.set_resource( "" ) == false );
	}

	// Check setting resource.
	{
		FlexID id;
		id.set_package( "fw.weapons" );

		BOOST_CHECK( id.set_resource( "sword.png" ) == true );
		BOOST_CHECK( id.get_resource() == "sword.png" );
	}

	// Check setting invalid resource will recover previous one.
	{
		FlexID id;
		id.set_package( "fw.weapons" );
		id.set_resource( "sword.png" );
		id.set_resource( "" );

		BOOST_CHECK( id.get_resource() == "sword.png" );
	}

	// Check changing package won't alter resource.
	{
		FlexID id;

		id.set_package( "fw.weapons" );
		id.set_resource( "sword.png" );
		id.set_package( "fw.another" );

		BOOST_CHECK( id.get_resource() == "sword.png" );
	}

	// Check for validity.
	{
		FlexID id;

		BOOST_CHECK( !id.is_valid_package() );
		BOOST_CHECK( !id.is_valid_resource() );

		id.set_package( "fw" );
		BOOST_CHECK( id.is_valid_package() );
		BOOST_CHECK( !id.is_valid_resource() );

		id.set_resource( "res" );
		BOOST_CHECK( id.is_valid_package() );
		BOOST_CHECK( id.is_valid_resource() );
	}

	// Check for full IDs.
	{
		// Both with package and resource.
		{
			FlexID id;
			id.set_package( "fw.package" );
			id.set_resource( "res.yml" );
			BOOST_CHECK( id.get() == "fw.package/res.yml" );
		}

		// With package only.
		{
			FlexID id;
			id.set_package( "fw.package" );
			BOOST_CHECK( id.get() == "fw.package" );
		}

		// Without anything.
		{
			FlexID id;
			BOOST_CHECK( id.get() == "" );
		}
	}

	// Check parsing full IDs.
	{
		FlexID id;

		BOOST_CHECK( id.parse( "fw.weapons/sword.png" ) == true );
		BOOST_CHECK( id.parse( "fw/sword.png" ) == true );
		BOOST_CHECK( id.parse( "fw.monsters.evil/sword.png" ) == true );

		BOOST_CHECK( id.parse( "" ) == false );
		BOOST_CHECK( id.parse( "/" ) == false );
		BOOST_CHECK( id.parse( "foo.bar/" ) == false );
		BOOST_CHECK( id.parse( "/foo.bar" ) == false );
		BOOST_CHECK( id.parse( ".foo.bar" ) == false );
		BOOST_CHECK( id.parse( "foo.bar." ) == false );
		BOOST_CHECK( id.parse( "foo.bar./meh.foo" ) == false );
		BOOST_CHECK( id.parse( ".foo.bar/meh.foo" ) == false );
	}

	// Check parsing faulty full ID recovers previous state.
	{
		FlexID id;

		id.parse( "fw.weapons/sword.png" );
		id.parse( "" );

		BOOST_CHECK( id.get() == "fw.weapons/sword.png" );
	}
}

/*BOOST_AUTO_TEST_CASE( Class ) {
	flex::Class cls( flex::FlexID( "fw.base/grass" ) );
	cls.set_name( "Grass" );
	cls.set_origin( sf::Vector3f( 1.f, 2.f, 3.f ) );

	flex::Resource tex0( flex::FlexID( "fw.base/0.png" ) );
	flex::Resource tex1( flex::FlexID( "fw.base/1.png" ) );
	flex::Resource tex2( flex::FlexID( "fw.base/2.png" ) );
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
	flex::Class cls( flex::FlexID( "fw.base/grass" ) );

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

	flex::Class cls0( flex::FlexID( "fw/class0" ) );
	flex::Class cls1( flex::FlexID( "fw/class1" ) );

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
	flex::Class cls0( flex::FlexID( "fw/class0" ) );
	flex::Class cls1( flex::FlexID( "fw/class1" ) );

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

	// Check constants.
	BOOST_CHECK( MessageMeta::get_type_size( MessageMeta::BYTE ) == sizeof( uint8_t ) );
	BOOST_CHECK( MessageMeta::get_type_size( MessageMeta::WORD ) == sizeof( uint16_t ) );
	BOOST_CHECK( MessageMeta::get_type_size( MessageMeta::DWORD ) == sizeof( uint32_t ) );
	BOOST_CHECK( MessageMeta::get_type_size( MessageMeta::STRING ) == sizeof( uint32_t ) );

	// Init state.
	MessageMeta meta;

	BOOST_CHECK( meta.get_num_fields() == 0 );
	BOOST_CHECK_THROW( meta.get_field_type( 0 ), std::out_of_range );
	BOOST_CHECK( meta.get_minimum_size() == 0 );

	// Add fields.
	meta.add_field( MessageMeta::BYTE );
	BOOST_CHECK( meta.get_minimum_size() == sizeof( uint8_t ) );
	meta.add_field( MessageMeta::WORD );
	BOOST_CHECK( meta.get_minimum_size() == sizeof( uint8_t ) + sizeof( uint16_t ) );
	meta.add_field( MessageMeta::DWORD );
	BOOST_CHECK( meta.get_minimum_size() == sizeof( uint8_t ) + sizeof( uint16_t ) + sizeof( uint32_t ) );
	meta.add_field( MessageMeta::STRING );
	BOOST_CHECK( meta.get_minimum_size() == sizeof( uint8_t ) + sizeof( uint16_t ) + sizeof( uint32_t ) + sizeof( uint32_t ) );

	BOOST_CHECK( meta.get_num_fields() == 4 );
	BOOST_CHECK( meta.get_field_type( 0 ) == MessageMeta::BYTE );
	BOOST_CHECK( meta.get_field_type( 1 ) == MessageMeta::WORD );
	BOOST_CHECK( meta.get_field_type( 2 ) == MessageMeta::DWORD );
	BOOST_CHECK( meta.get_field_type( 3 ) == MessageMeta::STRING );
	BOOST_CHECK_THROW( meta.get_field_type( 4 ), std::out_of_range );
}

BOOST_AUTO_TEST_CASE( FlexMessage ) {
	using namespace flex;

	// Build sample message meta info.
	MessageMeta meta;
	meta.add_field( MessageMeta::BYTE );
	meta.add_field( MessageMeta::WORD );
	meta.add_field( MessageMeta::DWORD );
	meta.add_field( MessageMeta::STRING );

	// Create message and check initial state.
	Message msg( meta );
	std::size_t offset( 0 );
	std::size_t size( sizeof( uint8_t ) + sizeof( uint16_t ) + sizeof( uint32_t ) + sizeof( uint32_t ) );

	BOOST_CHECK( msg.get_current_field() == 0 );
	BOOST_CHECK( msg.get_size() == size );
	BOOST_CHECK( msg.get_field_offset( 0 ) == offset ); offset += sizeof( uint8_t );
	BOOST_CHECK( msg.get_field_offset( 1 ) == offset ); offset += sizeof( uint16_t );
	BOOST_CHECK( msg.get_field_offset( 2 ) == offset ); offset += sizeof( uint32_t );
	BOOST_CHECK( msg.get_field_offset( 3 ) == offset ); offset += sizeof( uint32_t );
	BOOST_CHECK_THROW( msg.get_field_offset( 4 ), std::out_of_range );

	// Pack some stuff.
	BOOST_CHECK_THROW( msg << int16_t( 22 ), Message::ValueTypeMismatch );
	BOOST_CHECK_THROW( msg << int32_t( 22 ), Message::ValueTypeMismatch );
	BOOST_CHECK_NO_THROW( msg << int8_t( 22 ) );
	BOOST_CHECK( msg.get_size() == size );
	BOOST_CHECK( msg.get_current_field() == 1 );

	BOOST_CHECK_THROW( msg << int8_t( 22 ), Message::ValueTypeMismatch );
	BOOST_CHECK_THROW( msg << int32_t( 22 ), Message::ValueTypeMismatch );
	BOOST_CHECK_NO_THROW( msg << int16_t( 22 ) );
	BOOST_CHECK( msg.get_size() == size );
	BOOST_CHECK( msg.get_current_field() == 2 );

	BOOST_CHECK_THROW( msg << int8_t( 22 ), Message::ValueTypeMismatch );
	BOOST_CHECK_THROW( msg << int16_t( 22 ), Message::ValueTypeMismatch );
	BOOST_CHECK_NO_THROW( msg << int32_t( 22 ) );
	BOOST_CHECK( msg.get_size() == size );
	BOOST_CHECK( msg.get_current_field() == 3 );

	BOOST_CHECK_THROW( msg << int8_t( 22 ), Message::ValueTypeMismatch );
	BOOST_CHECK_THROW( msg << int16_t( 22 ), Message::ValueTypeMismatch );
	BOOST_CHECK_THROW( msg << int32_t( 22 ), Message::ValueTypeMismatch );
	BOOST_CHECK_NO_THROW( msg << "FOOBAR" );
	size += 6;
	BOOST_CHECK( msg.get_size() == size );
	BOOST_CHECK( msg.get_current_field() == 3 );

	// Move the current field pointer.
	BOOST_CHECK_NO_THROW( msg.set_next_field( msg.get_current_field() ) );

	BOOST_CHECK_NO_THROW( msg.set_next_field( 0 ) );
	BOOST_CHECK( msg.get_current_field() == 0 );
	BOOST_CHECK_NO_THROW( msg.set_next_field( 1 ) );
	BOOST_CHECK( msg.get_current_field() == 1 );
	BOOST_CHECK_NO_THROW( msg.set_next_field( 2 ) );
	BOOST_CHECK( msg.get_current_field() == 2 );
	BOOST_CHECK_NO_THROW( msg.set_next_field( 3 ) );
	BOOST_CHECK( msg.get_current_field() == 3 );

	BOOST_CHECK_THROW( msg.set_next_field( 4 ), std::out_of_range );

	// Check re-packing values.
	BOOST_CHECK_NO_THROW( msg.set_next_field( 0 ) );
	BOOST_CHECK( msg.get_current_field() == 0 );

	BOOST_CHECK_THROW( msg << int16_t( 22 ), Message::ValueTypeMismatch );
	BOOST_CHECK_THROW( msg << int32_t( 22 ), Message::ValueTypeMismatch );
	BOOST_CHECK_NO_THROW( msg << int8_t( 22 ) );
	BOOST_CHECK( msg.get_size() == size );
	BOOST_CHECK( msg.get_current_field() == 1 );

	BOOST_CHECK_NO_THROW( msg.set_next_field( 1 ) );
	BOOST_CHECK( msg.get_current_field() == 1 );

	BOOST_CHECK_THROW( msg << int8_t( 22 ), Message::ValueTypeMismatch );
	BOOST_CHECK_THROW( msg << int32_t( 22 ), Message::ValueTypeMismatch );
	BOOST_CHECK_NO_THROW( msg << int16_t( 22 ) );
	BOOST_CHECK( msg.get_size() == size );
	BOOST_CHECK( msg.get_current_field() == 2 );

	BOOST_CHECK_NO_THROW( msg.set_next_field( 2 ) );
	BOOST_CHECK( msg.get_current_field() == 2 );

	BOOST_CHECK_THROW( msg << int8_t( 22 ), Message::ValueTypeMismatch );
	BOOST_CHECK_THROW( msg << int16_t( 22 ), Message::ValueTypeMismatch );
	BOOST_CHECK_NO_THROW( msg << int32_t( 22 ) );
	BOOST_CHECK( msg.get_size() == size );
	BOOST_CHECK( msg.get_current_field() == 3 );

	// Check re-packing strings.
	BOOST_CHECK_NO_THROW( msg.set_next_field( 3 ) );
	BOOST_CHECK( msg.get_current_field() == 3 );

	// Pack same string again.
	BOOST_CHECK_THROW( msg << int8_t( 22 ), Message::ValueTypeMismatch );
	BOOST_CHECK_THROW( msg << int16_t( 22 ), Message::ValueTypeMismatch );
	BOOST_CHECK_THROW( msg << int32_t( 22 ), Message::ValueTypeMismatch );
	BOOST_CHECK_NO_THROW( msg << "FOOBAR" );
	BOOST_CHECK( msg.get_size() == size );
	BOOST_CHECK( msg.get_current_field() == 3 );
}*/
