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

flex.event:hook_command( "shutdown", on_shutdown_command )
