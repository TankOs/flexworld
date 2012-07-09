#include <FlexWorld/Entity.hpp>
#include <FlexWorld/Class.hpp>
#include <FlexWorld/FlexID.hpp>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE( TestEntity ) {
	using namespace fw;

	FlexID id;
	id.parse( "fw.weapons/sword" );

	Class sword_cls( id );
	sword_cls.set_hook( "hook", sf::Vector3f( 0, 0, 0 ) );
	sword_cls.set_hook( "fukk", sf::Vector3f( 0, 0, 0 ) );

	// Initial state.
	{
		Entity ent( sword_cls );

		BOOST_CHECK( ent.get_id() == 0 );
		BOOST_CHECK( &ent.get_class() == &sword_cls );
		BOOST_CHECK( ent.get_name() == sword_cls.get_name() );
		BOOST_CHECK( ent.get_amount() == 1 );
		BOOST_CHECK( ent.get_position() == sf::Vector3f( 0, 0, 0 ) );
		BOOST_CHECK( ent.get_rotation() == sf::Vector3f( 0, 0, 0 ) );
		BOOST_CHECK( ent.get_parent() == nullptr );
		BOOST_CHECK( ent.get_num_children() == 0 );
	}

	// Basic properties.
	{
		Entity ent( sword_cls );
		Entity other( sword_cls );

		ent.set_id( 123 );
		ent.set_amount( 333 );
		ent.set_position( sf::Vector3f( 4, 5, 6 ) );
		ent.set_rotation( sf::Vector3f( 7, 8, 9 ) );
		ent.set_parent( &other );

		BOOST_CHECK( ent.get_id() == 123 );
		BOOST_CHECK( ent.get_amount() == 333 );
		BOOST_CHECK( ent.get_position() == sf::Vector3f( 4, 5, 6 ) );
		BOOST_CHECK( ent.get_rotation() == sf::Vector3f( 7, 8, 9 ) );
		BOOST_CHECK( ent.get_parent() == &other );

		ent.set_parent( nullptr );
		BOOST_CHECK( ent.get_parent() == nullptr );
	}

	// Custom/class name.
	{
		Entity ent( sword_cls );
		std::string old_name( ent.get_name() );
		BOOST_CHECK( ent.get_name() != "CustomName" );

		ent.set_name( "CustomName" );
		BOOST_CHECK( ent.get_name() == "CustomName" );

		ent.reset_name();
		BOOST_CHECK( ent.get_name() == old_name );
	}

	// Attach and detach entities.
	{
		Entity parent( sword_cls );
		Entity new_parent( sword_cls );
		Entity child( sword_cls );
		Entity brother( sword_cls );

		// Single.
		parent.attach( child, "hook" );
		BOOST_CHECK( parent.get_num_children() == 1 );
		BOOST_CHECK( parent.get_num_children( "hook" ) == 1 );
		BOOST_CHECK( parent.get_num_children( "fukk" ) == 0 );
		BOOST_CHECK( parent.has_child( child ) == true );
		BOOST_CHECK( parent.has_child( child, "hook" ) == true );
		BOOST_CHECK( parent.has_child( child, "fukk" ) == false );
		BOOST_CHECK( &parent.get_child( "hook", 0 ) == &child );
		BOOST_CHECK( child.get_parent() == &parent );
		BOOST_CHECK( parent.get_child_hook( child ) == "hook" );
		BOOST_CHECK( &get_uppermost_parent( child ) == &parent );

		parent.detach( child );
		BOOST_CHECK( parent.get_num_children() == 0 );
		BOOST_CHECK( parent.get_num_children( "hook" ) == 0 );
		BOOST_CHECK( parent.has_child( child ) == false );
		BOOST_CHECK( parent.has_child( child, "hook" ) == false );
		BOOST_CHECK( child.get_parent() == nullptr );

		// Multiple, to same hook.
		parent.attach( child, "hook" );
		parent.attach( brother, "hook" );

		BOOST_CHECK( parent.get_num_children() == 2 );
		BOOST_CHECK( parent.get_num_children( "hook" ) == 2 );
		BOOST_CHECK( parent.get_num_children( "fukk" ) == 0 );
		BOOST_CHECK( parent.has_child( child ) == true );
		BOOST_CHECK( parent.has_child( brother ) == true );
		BOOST_CHECK( parent.has_child( child, "hook" ) == true );
		BOOST_CHECK( parent.has_child( child, "fukk" ) == false );
		BOOST_CHECK( parent.has_child( brother, "hook" ) == true );
		BOOST_CHECK( parent.has_child( brother, "fukk" ) == false );
		BOOST_CHECK( &parent.get_child( "hook", 0 ) == &child );
		BOOST_CHECK( &parent.get_child( "hook", 1 ) == &brother );
		BOOST_CHECK( parent.get_child_hook( child ) == "hook" );
		BOOST_CHECK( parent.get_child_hook( brother ) == "hook" );
		BOOST_CHECK( &get_uppermost_parent( child ) == &parent );
		BOOST_CHECK( &get_uppermost_parent( brother ) == &parent );

		BOOST_CHECK( child.get_parent() == &parent );
		BOOST_CHECK( brother.get_parent() == &parent );

		parent.detach( child );

		BOOST_CHECK( parent.get_num_children() == 1 );
		BOOST_CHECK( parent.get_num_children( "hook" ) == 1 );
		BOOST_CHECK( parent.get_num_children( "fukk" ) == 0 );
		BOOST_CHECK( parent.has_child( child ) == false );
		BOOST_CHECK( parent.has_child( brother ) == true );
		BOOST_CHECK( parent.has_child( child, "hook" ) == false );
		BOOST_CHECK( parent.has_child( child, "fukk" ) == false );
		BOOST_CHECK( parent.has_child( brother, "hook" ) == true );
		BOOST_CHECK( parent.has_child( brother, "fukk" ) == false );
		BOOST_CHECK( &parent.get_child( "hook", 0 ) == &brother );
		BOOST_CHECK( parent.get_child_hook( brother ) == "hook" );
		BOOST_CHECK( &get_uppermost_parent( brother ) == &parent );

		BOOST_CHECK( child.get_parent() == nullptr );
		BOOST_CHECK( brother.get_parent() == &parent );

		parent.detach( brother );

		BOOST_CHECK( parent.get_num_children() == 0 );
		BOOST_CHECK( parent.get_num_children( "hook" ) == 0 );
		BOOST_CHECK( parent.get_num_children( "fukk" ) == 0 );
		BOOST_CHECK( parent.has_child( child ) == false );
		BOOST_CHECK( parent.has_child( brother ) == false );
		BOOST_CHECK( parent.has_child( child, "hook" ) == false );
		BOOST_CHECK( parent.has_child( child, "fukk" ) == false );
		BOOST_CHECK( parent.has_child( brother, "hook" ) == false );
		BOOST_CHECK( parent.has_child( brother, "fukk" ) == false );

		BOOST_CHECK( child.get_parent() == nullptr );
		BOOST_CHECK( brother.get_parent() == nullptr );

		// Multiple, different hooks.
		parent.attach( child, "hook" );
		parent.attach( brother, "fukk" );

		BOOST_CHECK( parent.get_num_children() == 2 );
		BOOST_CHECK( parent.get_num_children( "hook" ) == 1 );
		BOOST_CHECK( parent.get_num_children( "fukk" ) == 1 );
		BOOST_CHECK( parent.has_child( child ) == true );
		BOOST_CHECK( parent.has_child( brother ) == true );
		BOOST_CHECK( parent.has_child( child, "hook" ) == true );
		BOOST_CHECK( parent.has_child( child, "fukk" ) == false );
		BOOST_CHECK( parent.has_child( brother, "hook" ) == false );
		BOOST_CHECK( parent.has_child( brother, "fukk" ) == true );
		BOOST_CHECK( &parent.get_child( "hook", 0 ) == &child );
		BOOST_CHECK( &parent.get_child( "fukk", 0 ) == &brother );
		BOOST_CHECK( parent.get_child_hook( child ) == "hook" );
		BOOST_CHECK( parent.get_child_hook( brother ) == "fukk" );
		BOOST_CHECK( &get_uppermost_parent( child ) == &parent );
		BOOST_CHECK( &get_uppermost_parent( brother ) == &parent );

		BOOST_CHECK( child.get_parent() == &parent );
		BOOST_CHECK( brother.get_parent() == &parent );

		parent.detach( brother );

		BOOST_CHECK( parent.get_num_children() == 1 );
		BOOST_CHECK( parent.get_num_children( "hook" ) == 1 );
		BOOST_CHECK( parent.get_num_children( "fukk" ) == 0 );
		BOOST_CHECK( parent.has_child( child ) == true );
		BOOST_CHECK( parent.has_child( brother ) == false );
		BOOST_CHECK( parent.has_child( child, "hook" ) == true );
		BOOST_CHECK( parent.has_child( child, "fukk" ) == false );
		BOOST_CHECK( parent.has_child( brother, "hook" ) == false );
		BOOST_CHECK( parent.has_child( brother, "fukk" ) == false );
		BOOST_CHECK( &parent.get_child( "hook", 0 ) == &child );
		BOOST_CHECK( parent.get_child_hook( child ) == "hook" );
		BOOST_CHECK( &get_uppermost_parent( child ) == &parent );

		BOOST_CHECK( child.get_parent() == &parent );
		BOOST_CHECK( brother.get_parent() == nullptr );

		parent.detach( child );

		BOOST_CHECK( parent.get_num_children() == 0 );
		BOOST_CHECK( parent.get_num_children( "hook" ) == 0 );
		BOOST_CHECK( parent.get_num_children( "fukk" ) == 0 );
		BOOST_CHECK( parent.has_child( child ) == false );
		BOOST_CHECK( parent.has_child( brother ) == false );
		BOOST_CHECK( parent.has_child( child, "hook" ) == false );
		BOOST_CHECK( parent.has_child( child, "fukk" ) == false );
		BOOST_CHECK( parent.has_child( brother, "hook" ) == false );
		BOOST_CHECK( parent.has_child( brother, "fukk" ) == false );

		BOOST_CHECK( child.get_parent() == nullptr );
		BOOST_CHECK( brother.get_parent() == nullptr );
	}
}
