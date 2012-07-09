--
-- Spawn command.
--

function on_spawn_command( args, client_id )
	if #args ~= 1 then
		fw.server:broadcast_chat_message( "Usage: /spawn CLASS_ID", "Status", "Server" )
		return
	end

	local class_id = args[1]
	local ent_id = fw.server:get_client_entity_id( client_id )
	local ent_pos, ent_planet = fw.world:get_entity_position( ent_id )

	fw.world:create_entity( class_id, ent_pos, ent_planet )

	fw.server:broadcast_chat_message( "Spawned " .. class_id .. ".", "Status", "Server" )
end

fw.event:hook_command( "spawn", on_spawn_command )
