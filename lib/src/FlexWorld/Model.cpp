#include <FlexWorld/Model.hpp>

#include <cmath>
#include <cassert>

namespace flex {

Model::Model() :
	m_scale_divisor( 1.0f )
{
}

std::size_t Model::get_num_meshes() const {
	return m_meshes.size();
}

void Model::add_mesh( const Mesh& mesh ) {
	m_meshes.push_back( mesh );
}

const Mesh& Model::get_mesh( std::size_t index ) const {
	assert( index < m_meshes.size() );

	return m_meshes[index];
}

void Model::set_block_scale_divisor( float divisor ) {
	assert( divisor >= 1.0f );
	m_scale_divisor = std::max( 1.0f, divisor );
}

float Model::get_block_scale_divisor() const {
	return m_scale_divisor;
}

}
