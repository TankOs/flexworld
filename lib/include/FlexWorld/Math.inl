namespace flex {

template <class T>
char signum( const T& val ) {
	return static_cast<char>( (T( 0 ) < val) - (val < T( 0 )) );
}

}
