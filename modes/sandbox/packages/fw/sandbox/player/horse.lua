--
-- Horse fun.
--

function on_dwarf_use_horse( entity_id, actor_id, client_id )
	flex.server:broadcast_chat_message( "I'm not in a mood for a ride, buddy.", "Status", "Pony" )
end

flex.event:hook_class_event( flex.Event.Class.USE, "fw.sandbox.player/dwarf_male", on_dwarf_use_horse )
