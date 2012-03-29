--
-- Generic administrator commands.
--

function on_shutdown_command( client, args )
	if #args < 1 then
		flex.comm:send_server_message( "Are you sure? Type /shutdown yes", client )
	elseif args[1] == "yes" then
		local username = flex.server:get_username( client )

		flex.comm:broadcast_server_message( "Server is shutting down, requested by " .. username .. "." )
		flex.server:shutdown()
	end
end

function on_rehash_command( client, args )
	flex.comm:broadcast_server_message( "Game paused, stand by..." )
	flex.server:rehash()
	flex.comm:broadcast_server_message( "Game continued, have fun!" )
end

flex.event:hook_command( "shutdown", on_shutdown_command )
flex.event:hook_command( "rehash", on_rehash_command )
