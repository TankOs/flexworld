#include <ChunkPreparerReader.hpp>
#include <PlanetDrawable.hpp>
#include <ResourceManager.hpp>

#include <FlexWorld/Planet.hpp>
#include <FlexWorld/Types.hpp>

#include <FWSG/Renderer.hpp>
#include <FWMS/Router.hpp>
#include <FWMS/Reader.hpp>
#include <FWMS/Message.hpp>
#include <boost/test/unit_test.hpp>

struct DummyReader : public ms::Reader {
	DummyReader() :
		ms::Reader(),
		num_messages_handled( 0 )
	{
	}

	void handle_message( const ms::Message& message ) {
		if( message.get_id() == ms::string_hash( "chunk_prepared" ) ) {
			const fw::ChunkVector* property = message.find_property<fw::ChunkVector>( ms::string_hash( "position" ) );

			BOOST_REQUIRE( property != nullptr );
			BOOST_CHECK( *property == fw::ChunkVector( 1, 2, 3 ) );

			++num_messages_handled;
		}
	}

	std::size_t num_messages_handled;
};

BOOST_AUTO_TEST_CASE( TestChunkPreparerReader ) {
	// Initial state and basic properties.
	{
		ChunkPreparerReader reader;

		BOOST_CHECK( reader.is_running() == false );

		sg::Renderer renderer;
		ResourceManager resource_manager;
		fw::Planet planet( "foobar", fw::PlanetSize( 1, 1, 1 ), fw::ChunkSize( 2, 2, 2 ) );

		PlanetDrawable::Ptr planet_drawable = PlanetDrawable::create( planet, resource_manager, renderer );
		reader.set_planet_drawable( planet_drawable );

		BOOST_CHECK( reader.get_planet_drawable() == planet_drawable );
	}

	/*
	// Send "chunk_created" message.
	{
		ms::Router router;
		router.create_reader<ChunkPreparerReader>();
		DummyReader& dummy_reader = router.create_reader<DummyReader>();

		std::shared_ptr<ms::Message> message = std::make_shared<ms::Message>( ms::string_hash( "chunk_created" ) );
		message->set_property( ms::string_hash( "position" ), fw::ChunkVector( 1, 2, 3 ) );

		router.enqueue_message( message );
		router.process_queue();

		BOOST_CHECK( dummy_reader.num_messages_handled == 1 );
	}

	// Send "chunk_updated" message.
	{
		ms::Router router;
		router.create_reader<ChunkPreparerReader>();
		DummyReader& dummy_reader = router.create_reader<DummyReader>();

		std::shared_ptr<ms::Message> message = std::make_shared<ms::Message>( ms::string_hash( "chunk_updated" ) );
		message->set_property( ms::string_hash( "position" ), fw::ChunkVector( 1, 2, 3 ) );

		router.enqueue_message( message );
		router.process_queue();

		BOOST_CHECK( dummy_reader.num_messages_handled == 1 );
	}
	*/

	// Launch and stop worker thread.
	{
		ChunkPreparerReader reader;

		reader.launch();
		BOOST_CHECK( reader.is_running() == true );

		reader.stop();
		BOOST_CHECK( reader.is_running() == false );
	}
}
