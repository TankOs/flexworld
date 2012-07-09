--
-- Generic administrator commands.
--

function on_shutdown_command( args, sender )
	-- TODO Replace by send_server_message!
	--
	if #args < 1 then
		flex.server:broadcast_chat_message( "Are you sure? Type /shutdown yes", "Status", "Server" )
	elseif args[1] == "yes" then
		local username = flex.server:get_client_username( sender )

		flex.server:broadcast_chat_message( "Server is shutting down, requested by " .. username .. ".", "Status", "Server" )
		--flex.server:shutdown()
	end
end

function on_rehash_command( args, sender )
	flex.server:broadcast_chat_message( "Scripts rehashed.", "Status", "Server" )
end

function on_help_command( args, sender )
	flex.server:broadcast_chat_message( "Commands: /rehash /shutdown", "Status", "Server" )
end

flex.event:hook_command( "shutdown", on_shutdown_command )
flex.event:hook_command( "rehash", on_rehash_command )
flex.event:hook_command( "help", on_help_command )
