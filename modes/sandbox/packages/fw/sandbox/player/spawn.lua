--
-- Spawn command.
--

function on_spawn_command( args, client_id )
	if #args ~= 1 then
		flex.server:broadcast_chat_message( "Usage: /spawn CLASS_ID", "Status", "Server" )
		return
	end

	local class_id = args[1]
	local ent_id = flex.server:get_client_entity_id( client_id )
	local ent_pos, ent_planet = flex.world:get_entity_position( ent_id )

	flex.world:create_entity( class_id, ent_pos, ent_planet )
end

flex.event:hook_command( "spawn", on_spawn_command )
