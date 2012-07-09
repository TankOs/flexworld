--
-- Containers and backpacks.
--

function on_backpack_use( entity_id, actor_id, client_id )
	local ent_cls_id = fw.world:get_entity_class_id( entity_id )

	if ent_cls_id ~= "fw.items.containers/backpack" then
		return
	end
end

fw.event:hook_event( fw.Event.USE, on_backpack_use )
