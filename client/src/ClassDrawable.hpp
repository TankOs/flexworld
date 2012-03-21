#pragma once

#include "BufferObjectGroup.hpp"

#include <FWSG/Drawable.hpp>
#include <FWSG/StepProxy.hpp>
#include <vector>

class ResourceManager;

namespace flex {
class Class;
}

/** Drawable for rendering FlexWorld classes.
 */
class ClassDrawable : public sg::Drawable {
	public:
		typedef std::shared_ptr<ClassDrawable> Ptr; ///< Shared pointer.
		typedef std::shared_ptr<const ClassDrawable> PtrConst; ///< Shared pointer.

		/** Create drawable.
		 * @param renderer Renderer.
		 * @param resource_manager Resource manager.
		 * @return ClassDrawable.
		 */
		static Ptr create( sg::Renderer& renderer, ResourceManager& resource_manager );

		/** Set class to render.
		 * Will auto-load missing resources by calling the prepare_*() methods of
		 * the resource manager
		 * @param cls Class (reference is stored internally).
		 */
		void set_class( const flex::Class& cls );

	protected:
		/** Ctor.
		 * @param renderer Renderer.
		 * @param resource_manager Resource manager.
		 */
		ClassDrawable( sg::Renderer& renderer, ResourceManager& resource_manager );

	private:
		typedef std::vector<sg::StepProxy::Ptr> StepArray;

		void handle_update_render_state();
		void handle_update();

		void invalidate_steps();

		StepArray m_steps;

		ResourceManager& m_resource_manager;
		const flex::Class* m_class;
};
