#include "ClassDrawable.hpp"
#include "ResourceManager.hpp"

#include <FlexWorld/Class.hpp>
#include <FlexWorld/FlexID.hpp>

#include <FWSG/Renderer.hpp>
#include <iostream>

ClassDrawable::Ptr ClassDrawable::create( sg::Renderer& renderer, ResourceManager& resource_manager ) {
	Ptr drawable( new ClassDrawable( renderer, resource_manager ) );

	return drawable;
}

ClassDrawable::ClassDrawable( sg::Renderer& renderer, ResourceManager& resource_manager ) :
	Drawable( renderer ),
	m_resource_manager( resource_manager ),
	m_class( nullptr )
{
}

void ClassDrawable::set_class( const flex::Class& cls ) {
	m_class = &cls;

	// Apply scale.
	sg::Transform trans = get_local_transform();
	trans.set_scale( m_class->get_scale() );

	set_local_transform( trans );

	queue_update();
}

void ClassDrawable::handle_update_render_state() {
	invalidate_steps();
}

void ClassDrawable::handle_update() {
	invalidate_steps();
}

void ClassDrawable::invalidate_steps() {
	// Remove all previous steps.
	m_steps.clear();

	// Find buffer object group.
	const flex::FlexID& model_id = m_class->get_model().get_id();

	BufferObjectGroup::PtrConst group = m_resource_manager.find_buffer_object_group( model_id );

	// Try to prepare group if it doesn't exist yet.
	if( group == nullptr ) {
		bool result = m_resource_manager.prepare_buffer_object_group( model_id );

		assert( result == true );
		if( !result ) {
			std::cerr << "ERROR: Failed to prepare group for model " << model_id.get() << "." << std::endl;
			return;
		}

		group = m_resource_manager.find_buffer_object_group( model_id );
		assert( group != nullptr );
	}

	// Get model. At this point it exists for sure due to the previous call to
	// prepare_buffer_object_group().
	std::shared_ptr<const flex::Model> model = m_resource_manager.find_model( model_id );
	assert( model != nullptr );

	// Iterate over buffer objects.
	for( std::size_t bo_idx = 0; bo_idx < group->get_num_buffer_objects(); ++bo_idx ) {
		// Get texture ID.
		const flex::FlexID& texture_id = m_class->get_texture(
			model->get_mesh( bo_idx ).get_texture_slot()
		).get_id();

		// Fetch texture.
		std::shared_ptr<const sf::Texture> texture = m_resource_manager.find_texture( texture_id );

		// Try to prepare texture if it doesn't exist yet.
		if( texture == nullptr ) {
			bool result = m_resource_manager.prepare_texture( texture_id );

			assert( result == true );
			if( !result ) {
				std::cerr << "ERROR: Failed to prepare texture " << texture_id.get() << "." << std::endl;
				return;
			}

			texture = m_resource_manager.find_texture( texture_id );
			assert( texture != nullptr );
		}

		// Prepare render state.
		sg::RenderState state = get_render_state();
		state.texture = texture;

		// Create step.
		m_steps.push_back( get_renderer().create_step( state, get_global_transform(), group->get_buffer_object( bo_idx ) ) );
	}
}

