namespace fw {

template <class T>
Log& Log::operator<<( const T& data ) {
	if( m_next_level < m_min_level ) {
		return *this;
	}

	if( m_console_logging ) {
		std::cout << data;
	}

	return *this;
}

}
