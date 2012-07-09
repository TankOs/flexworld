--
-- Horse fun.
--

function on_dwarf_use_horse( entity_id, actor_id, client_id )
	fw.server:broadcast_chat_message( "I'm not in a mood for a ride, buddy.", "Status", "Pony" )

	fw.world:create_entity( "fw.items.tools/shovel", entity_id, "rhand" )
end

fw.event:hook_event( fw.Event.USE, on_dwarf_use_horse )
