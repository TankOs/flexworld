#include <FlexWorld/RefLock.hpp>

#include <boost/test/unit_test.hpp>

enum { NUM_LOCKS = 200 };
enum { NUM_THREADS = 50 };

bool g_wait = true;

void thread_func( flex::RefLock* lock ) {
	// Wait until all threads are ready.
	while( g_wait ) {}

	// Lock.
	for( std::size_t iter = 0; iter < NUM_LOCKS; ++iter ) {
		lock->lock();
		BOOST_CHECK( lock->get_usage_count() == iter + 1 );
	}

	// Unlock.
	for( std::size_t iter = 0; iter < NUM_LOCKS; ++iter ) {
		lock->unlock();
		BOOST_CHECK( lock->get_usage_count() == NUM_LOCKS - iter - 1 );
	}
}

BOOST_AUTO_TEST_CASE( TestRefLock ) {
	using namespace flex;

	// Initial state.
	{
		RefLock lock;

		BOOST_CHECK( lock.get_usage_count() == 0 );
	}

	// Lock and unlock from one thread.
	{
		RefLock lock;

		lock.lock();
		BOOST_CHECK( lock.get_usage_count() == 1 );

		lock.unlock();
		BOOST_CHECK( lock.get_usage_count() == 0 );

		lock.lock();
		lock.lock();
		lock.lock();
		lock.lock();
		BOOST_CHECK( lock.get_usage_count() == 4 );

		lock.unlock();
		lock.unlock();
		lock.unlock();
		lock.unlock();
		BOOST_CHECK( lock.get_usage_count() == 0 );
	}

	// Lock and unlock from multiple threads.
	{
		RefLock lock;

		std::vector<std::shared_ptr<boost::thread> > thread_pool;

		for( std::size_t thread_idx = 0; thread_idx < NUM_THREADS; ++thread_idx ) {
			thread_pool.push_back( std::shared_ptr<boost::thread>( new boost::thread( std::bind( &thread_func, &lock ) ) ) );
		}

		// Signal threads to run.
		g_wait = false;

		// Wait for threads to finish.
		for( std::size_t thread_idx = 0; thread_idx < NUM_THREADS; ++thread_idx ) {
			thread_pool[thread_idx]->join();
		}
	}
}
