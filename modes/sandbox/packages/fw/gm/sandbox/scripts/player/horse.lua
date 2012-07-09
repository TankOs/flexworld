--
-- Horse fun.
--

function on_dwarf_use_horse( entity_id, actor_id, client_id )
	if fw.world:get_entity_class_id( entity_id ) ~= "fw.chars.animals/horse" then
		return
	end

	fw.server:broadcast_chat_message( fw.server:get_client_username( client_id ) .. ": I'm not in a mood for a ride, buddy.", "Status", "Pony" )
end

fw.event:hook_event( fw.Event.USE, on_dwarf_use_horse )
