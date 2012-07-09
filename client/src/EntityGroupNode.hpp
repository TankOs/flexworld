#pragma once

#include <FWSG/Node.hpp>

#include <map>
#include <cstdint>

namespace sg {
class Renderer;
}

namespace fw {
class Entity;
}

class ResourceManager;
class ClassDrawable;

/** FWSG drawable for rendering multiple entities.
 *
 * The group node is responsible for rendering entities in hierarchies.
 * Entities just have to be added to render them correctly depending on their
 * position, if attached or not.
 *
 * Entities that are being (re-)attached have to be re-inserted to the group
 * node. The update call will NOT handle that case.
 */
class EntityGroupNode : public sg::Node {
	public:
		typedef std::shared_ptr<EntityGroupNode> Ptr; ///< Shared pointer.
		typedef std::shared_ptr<EntityGroupNode> PtrConst; ///< Shared pointer to const.

		/** Create.
		 * @param resource_manager Resource manager.
		 * @param renderer Renderer.
		 * @return EntityGroupNode.
		 */
		static Ptr create( ResourceManager& resource_manager, sg::Renderer& renderer );

		/** Add entity.
		 * Undefined behaviour if entity has been added before. If entity is a
		 * child, all parent entities must have been added, too.
		 * @param entity Entity.
		 */
		void add_entity( const fw::Entity& entity );

		/** Update entity.
		 * Undefined behaviour if entity hasn't been added before.
		 * @param entity Entity (ptr is stored).
		 */
		void update_entity( const fw::Entity& entity );

		/** Remove entity (do not render anymore).
		 * @param entity Entity.
		 */
		void remove_entity( const fw::Entity& entity );

	private:
		typedef std::map<uint32_t, std::shared_ptr<ClassDrawable>> EntityClassDrawableMap;

		EntityGroupNode( ResourceManager& resource_manager, sg::Renderer& renderer );

		EntityClassDrawableMap m_class_drawables;

		ResourceManager& m_resource_manager;
		sg::Renderer& m_renderer;
};
