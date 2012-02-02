#include <FlexWorld/Model.hpp>

#include <cassert>

namespace flex {

Model::Model() {
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

}
