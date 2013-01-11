#include <FlexWorld/Controllers/EntityWatchdog.hpp>

#include <FWCS/Entity.hpp>
#include <FWMS/Router.hpp>
#include <FWMS/Reader.hpp>
#include <FWMS/Message.hpp>
#include <SFML/System/Time.hpp>
#include <boost/test/unit_test.hpp>

using namespace fw;
using fw::ctrl::EntityWatchdog;

class EntityWatchdogReader : public ms::Reader {
	public:
		EntityWatchdogReader() :
			ms::Reader(),
			num_messages_received( 0 ),
			expected_fields( fw::ctrl::EntityWatchdog::UNCHANGED )
		{
		}

		void handle_message( const ms::Message& message ) {
			BOOST_REQUIRE( message.get_id() == ms::string_hash( "entity_change" ) );

			++num_messages_received;

			auto* entity_id = message.find_property<fw::EntityID>( ms::string_hash( "id" ) );
			auto* fields = message.find_property<int>( ms::string_hash( "fields" ) );
			const auto* const* snapshot = message.find_property<const EntityWatchdog::Snapshot*>( ms::string_hash( "snapshot" ) );

			BOOST_REQUIRE( entity_id != nullptr );
			BOOST_REQUIRE( fields != nullptr );
			BOOST_REQUIRE( snapshot != nullptr );

			BOOST_CHECK( (*snapshot)->position == expected_position );
			BOOST_CHECK( *fields == expected_fields );
		}

		sf::Vector3f expected_position;
		std::size_t num_messages_received;
		int expected_fields;
};

cs::Entity create_entity_watchdog_entity( ms::Router& router ) {
	cs::Entity entity;

	entity.create_property( "watch_router", &router );
	entity.create_property( "fw_entity_id", fw::EntityID{ 1337 } );
	entity.create_property( "position", sf::Vector3f{ 1.0f, 2.0f, 3.0f } );

	return std::move( entity );
}

BOOST_AUTO_TEST_CASE( TestEntityWatchdogController ) {
	// Requirements.
	{
		const auto& req = ctrl::EntityWatchdog::get_requirements();

		BOOST_REQUIRE( req.get_num_requirements() == 3 );
		BOOST_REQUIRE( req.get_property_requirement( 0 ) == cs::ControllerRequirements::PropertyRequirement( "fw_entity_id", typeid( fw::EntityID ).name(), true ) );
		BOOST_REQUIRE( req.get_property_requirement( 1 ) == cs::ControllerRequirements::PropertyRequirement( "watch_router", typeid( ms::Router* ).name(), true ) );
		BOOST_REQUIRE( req.get_property_requirement( 2 ) == cs::ControllerRequirements::PropertyRequirement( "position", typeid( sf::Vector3f ).name(), true ) );
	}

	// Initial message.
	{
		static const sf::Vector3f POSITION{ 5.0f, 10.0f, 100.0f };

		ms::Router router;
		auto& reader = router.create_reader<EntityWatchdogReader>();

		cs::Entity entity = create_entity_watchdog_entity( router );
		*entity.find_property<sf::Vector3f>( "position" ) = POSITION;

		reader.expected_position = POSITION;
		reader.expected_fields = EntityWatchdog::ChangeFieldFlag::ALL;

		ctrl::EntityWatchdog controller{ entity };
		router.process_queue();

		BOOST_CHECK( &controller.get_entity() == &entity );
		BOOST_CHECK( reader.num_messages_received == 1 );
	}

	// Execute, no change.
	{
		static const sf::Vector3f POSITION{ 5.0f, 10.0f, 100.0f };

		ms::Router router;
		auto& reader = router.create_reader<EntityWatchdogReader>();

		cs::Entity entity = create_entity_watchdog_entity( router );
		*entity.find_property<sf::Vector3f>( "position" ) = POSITION;

		reader.expected_position = POSITION;
		reader.expected_fields = EntityWatchdog::ChangeFieldFlag::ALL;

		ctrl::EntityWatchdog controller{ entity };

		router.process_queue();
		controller.execute( sf::milliseconds( 234 ) );

		BOOST_CHECK( reader.num_messages_received == 1 );
	}

	// Execute, position change.
	{
		static const sf::Vector3f POSITION{ 5.0f, 10.0f, 100.0f };
		static const sf::Vector3f NEXT_POSITION{ 3.0f, 33.0f, 333.0f };

		ms::Router router;
		auto& reader = router.create_reader<EntityWatchdogReader>();

		cs::Entity entity = create_entity_watchdog_entity( router );
		*entity.find_property<sf::Vector3f>( "position" ) = POSITION;

		reader.expected_position = POSITION;
		reader.expected_fields = EntityWatchdog::ChangeFieldFlag::ALL;

		ctrl::EntityWatchdog controller{ entity };
		router.process_queue();

		// Next position.
		*entity.find_property<sf::Vector3f>( "position" ) = NEXT_POSITION;
		reader.expected_position = NEXT_POSITION;
		reader.expected_fields = EntityWatchdog::ChangeFieldFlag::POSITION;

		controller.execute( sf::milliseconds( 234 ) );
		router.process_queue();

		BOOST_CHECK( reader.num_messages_received == 2 );
	}
}
