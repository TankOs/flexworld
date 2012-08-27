#include "ChunkPreparerReader.hpp"
#include "PlanetDrawable.hpp"

#include <FlexWorld/Types.hpp>

#include <FWMS/Message.hpp>
#include <FWMS/Router.hpp>
#include <FWMS/Hash.hpp>
#include <memory>
#include <cassert>
#include <iostream>

ChunkPreparerReader::ChunkPreparerReader() :
	ms::Reader(),
	m_is_running( false ),
	m_terminate( false )
{
}

ChunkPreparerReader::~ChunkPreparerReader() {
	if( is_running() ) {
		stop();
	}
}

void ChunkPreparerReader::handle_message( const ms::Message& message ) {
	static const ms::HashValue CHUNK_CREATED_ID = ms::string_hash( "chunk_created" );
	static const ms::HashValue CHUNK_UPDATED_ID = ms::string_hash( "chunk_updated" );
	//static const ms::HashValue CHUNK_PREPARED_ID = ms::string_hash( "chunk_prepared" );
	static const ms::HashValue POSITION_PROP_ID = ms::string_hash( "position" );

	if(
		message.get_id() == CHUNK_CREATED_ID ||
		message.get_id() == CHUNK_UPDATED_ID
	) {
		const fw::ChunkVector* chunk_position = message.find_property<fw::ChunkVector>( POSITION_PROP_ID );
		ms::Router* router = get_router();

		assert( router != nullptr );
		assert( chunk_position != nullptr );

		{
			boost::lock_guard<boost::mutex> lock( m_mutex );
			if( m_is_running == false ) {
				std::cout << "WARNING: ChunkPreparerReader got a new chunk, but the worker is not running!" << std::endl;
			}
			else {
				boost::lock_guard<boost::mutex> data_lock( m_data_mutex );

				m_chunks.push_back( *chunk_position );
				m_condition.notify_one();
			}
		}
	}
}

bool ChunkPreparerReader::is_running() const {
	bool tmp_is_running = false;

	{
		boost::lock_guard<boost::mutex> lock( m_mutex );
		tmp_is_running = m_is_running;
	}

	return tmp_is_running;
}

void ChunkPreparerReader::launch() {
	{
		boost::lock_guard<boost::mutex> lock( m_mutex );
		assert( m_is_running == false );
		m_is_running = true;
	}

	m_thread = boost::thread( &ChunkPreparerReader::thread_func, this );
}

void ChunkPreparerReader::stop() {
	{
		boost::lock_guard<boost::mutex> lock( m_mutex );

		if( m_is_running ) {
			m_terminate = true;
			m_condition.notify_one();
		}
	}

	m_thread.join();
}

void ChunkPreparerReader::thread_func() {
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

		if( m_chunks.size() == 0 ) {
			// Wait for data to arrive.
			m_data_mutex.unlock();
			m_condition.wait( condition_lock );
			continue;
		}

		// Fetch next chunk position.
		fw::ChunkVector chunk_position = m_chunks.front();
		m_chunks.pop_front();

		m_data_mutex.unlock();

		// Give to renderer.
		m_planet_drawable->prepare_chunk( chunk_position );
	}

	{
		boost::lock_guard<boost::mutex> data_lock( m_data_mutex );
		boost::lock_guard<boost::mutex> lock( m_mutex );

		m_is_running = false;
		m_terminate = true;
		m_chunks.clear();
	}
}

void ChunkPreparerReader::set_planet_drawable( std::shared_ptr<PlanetDrawable> planet_drawable ) {
	boost::lock_guard<boost::mutex> lock( m_mutex );
	m_planet_drawable = planet_drawable;
}

std::shared_ptr<PlanetDrawable> ChunkPreparerReader::get_planet_drawable() const {
	std::shared_ptr<PlanetDrawable> planet_drawable;

	{
		boost::lock_guard<boost::mutex> lock( m_mutex );
		planet_drawable = m_planet_drawable;
	}

	return planet_drawable;
}
