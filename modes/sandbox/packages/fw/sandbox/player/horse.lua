--
-- Horse fun.
--

function on_dwarf_use_horse( entity_id, actor_id, client_id )
	flex.server:broadcast_chat_message( "I'm not in a mood for a ride, buddy.", "Status", "Pony" )

	flex.world:create_entity( "fw.tools/shovel", entity_id, "rhand" )
end

flex.event:hook_event( flex.Event.USE, on_dwarf_use_horse )
