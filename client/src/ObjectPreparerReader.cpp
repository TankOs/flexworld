#include "ObjectPreparerReader.hpp"
#include "PlanetDrawable.hpp"
#include "EntityGroupNode.hpp"

#include <FlexWorld/World.hpp>
#include <FlexWorld/LockFacility.hpp>
#include <FlexWorld/Types.hpp>

#include <FWMS/Message.hpp>
#include <FWMS/Router.hpp>
#include <FWMS/Hash.hpp>
#include <memory>
#include <cassert>
#include <iostream>

ObjectPreparerReader::ObjectPreparerReader() :
	ms::Reader(),
	m_world( nullptr ),
	m_lock_facility( nullptr ),
	m_is_running( false ),
	m_terminate( false )
{
}

ObjectPreparerReader::~ObjectPreparerReader() {
	if( is_running() ) {
		stop();
	}
}

void ObjectPreparerReader::handle_message( const ms::Message& message ) {
	static const ms::HashValue CHUNK_INVALIDATED_ID = ms::string_hash( "chunk_invalidated" );
	static const ms::HashValue ENTITY_INVALIDATED_ID = ms::string_hash( "entity_invalidated" );
	static const ms::HashValue POSITION_PROP_ID = ms::string_hash( "position" );
	static const ms::HashValue ID_PROP_ID = ms::string_hash( "id" );

	if( message.get_id() == CHUNK_INVALIDATED_ID ) {
		const fw::ChunkVector* chunk_position = message.find_property<fw::ChunkVector>( POSITION_PROP_ID );
		ms::Router* router = get_router();

		assert( router != nullptr );
		assert( chunk_position != nullptr );

		{
			boost::lock_guard<boost::mutex> lock( m_mutex );

			if( m_is_running == false ) {
				std::cout << "WARNING: ObjectPreparerReader got a new chunk, but the worker is not running!" << std::endl;
			}
			else {
				boost::lock_guard<boost::mutex> data_lock( m_data_mutex );

				m_chunks.push_back( *chunk_position );
				m_condition.notify_one();
			}
		}
	}
	else if( message.get_id() == ENTITY_INVALIDATED_ID ) {
		const fw::EntityID* entity_id = message.find_property<fw::EntityID>( ID_PROP_ID );
		ms::Router* router = get_router();

		assert( entity_id != nullptr );
		assert( router != nullptr );

		{
			boost::lock_guard<boost::mutex> lock( m_mutex );

			if( m_is_running == false ) {
				std::cout << "WARNING: ObjectPreparerReader got a new entity, but the worker is not running!" << std::endl;
			}
			else {
				boost::lock_guard<boost::mutex> data_lock( m_data_mutex );

				m_entities.push_back( *entity_id );
				m_condition.notify_one();
			}
		}
	}
}

bool ObjectPreparerReader::is_running() const {
	bool tmp_is_running = false;

	{
		boost::lock_guard<boost::mutex> lock( m_mutex );
		tmp_is_running = m_is_running;
	}

	return tmp_is_running;
}

void ObjectPreparerReader::launch() {
	{
		boost::lock_guard<boost::mutex> lock( m_mutex );
		assert( m_is_running == false );
		m_is_running = true;
	}

	m_thread = boost::thread( &ObjectPreparerReader::thread_func, this );
}

void ObjectPreparerReader::stop() {
	{
		boost::lock_guard<boost::mutex> lock( m_mutex );

		if( m_is_running ) {
			m_terminate = true;
			m_condition.notify_one();
		}
	}

	m_thread.join();

	m_chunks.clear();
	m_entities.clear();
}

void ObjectPreparerReader::thread_func() {
	boost::unique_lock<boost::mutex> condition_lock( m_condition_mutex );

	while( true ) {
		// Cancel?
		{
			boost::lock_guard<boost::mutex> lock( m_mutex );

			if( m_terminate ) {
				break;
			}
		}

		m_data_mutex.lock();

		if( m_chunks.size() == 0 && m_entities.size() == 0 ) {
			// Wait for data to arrive.
			m_data_mutex.unlock();
			m_condition.wait( condition_lock );
			continue;
		}

		if( m_chunks.size() > 0 ) {
			// Fetch next chunk position.
			fw::ChunkVector chunk_position = m_chunks.front();
			m_chunks.pop_front();

			m_data_mutex.unlock();

			// Give to renderer.
			m_planet_drawable->prepare_chunk( chunk_position );
		}
		else if( m_entities.size() > 0 ) {
			assert( m_world != nullptr );
			assert( m_lock_facility != nullptr );
			assert( m_entity_group_node != nullptr );

			// Fetch next entity ID.
			fw::EntityID entity_id = m_entities.front();
			m_entities.pop_front();

			m_data_mutex.unlock();

			// Fetch entity from world.
			m_lock_facility->lock_world( true );

			const fw::Entity* entity = m_world->find_entity( entity_id );

			if( entity ) {
				m_entity_group_node->add_entity( *entity );
			}
			else {
				std::cout << "WARNING: Entity " << entity_id << " shall be invalidated, but it wasn't found in the world." << std::endl;
			}

			m_lock_facility->lock_world( false );
		}
	}

	{
		boost::lock_guard<boost::mutex> data_lock( m_data_mutex );
		boost::lock_guard<boost::mutex> lock( m_mutex );

		m_is_running = false;
		m_terminate = true;
		m_chunks.clear();
	}
}

void ObjectPreparerReader::set_planet_drawable( std::shared_ptr<PlanetDrawable> planet_drawable ) {
	boost::lock_guard<boost::mutex> lock( m_mutex );
	m_planet_drawable = planet_drawable;
}

void ObjectPreparerReader::set_world( const fw::World& world ) {
	m_world = &world;
}

void ObjectPreparerReader::set_lock_facility( fw::LockFacility& lock_facility ) {
	m_lock_facility = &lock_facility;
}

void ObjectPreparerReader::set_entity_group_node( std::shared_ptr<EntityGroupNode> entity_group_node ) {
	m_entity_group_node = entity_group_node;
}
