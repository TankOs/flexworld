#include <FlexWorld/Model.hpp>

#include <cmath>
#include <cassert>

namespace fw {

Model::Model() :
	m_bounding_box( 0, 0, 0, 0, 0, 0 ),
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

void Model::set_bounding_box( const util::FloatCuboid& box ) {
	assert( box.x >= 0.0f );
	assert( box.y >= 0.0f );
	assert( box.z >= 0.0f );

	m_bounding_box = box;
}

const util::FloatCuboid& Model::get_bounding_box() const {
	return m_bounding_box;
}

const sf::FloatRect& Model::get_face_coverage( Face face ) const {
	assert( face < NUM_FACES && face >= 0 );

	return m_coverages[face];
}

void Model::set_face_coverage( Face face, const sf::FloatRect& coverage ) {
	assert( face < NUM_FACES && face >= 0 );
	assert( coverage.left >= 0.0f );
	assert( coverage.top >= 0.0f );
	assert( coverage.width >= 0.0f );
	assert( coverage.height >= 0.0f );

	m_coverages[face] = coverage;
}

}
