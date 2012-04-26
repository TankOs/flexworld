--
-- Spawn command.
--

function on_spawn_command( args, client_id )
	if #args ~= 1 then
		flex.server:broadcast_chat_message( "Usage: /spawn CLASS_ID", "Status", "Server" )
		return
	end

	flex.world:create_entity( args[1], {0, 0, 0}, "construct" ) -- TODO Replace by player planet + position.
end

flex.event:hook_command( "spawn", on_spawn_command )
