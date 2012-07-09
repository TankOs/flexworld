--
-- Generic administrator commands.
--

function on_shutdown_command( args, sender )
	-- TODO Replace by send_server_message!
	--
	if #args < 1 then
		fw.server:broadcast_chat_message( "Are you sure? Type /shutdown yes", "Status", "Server" )
	elseif args[1] == "yes" then
		local username = fw.server:get_client_username( sender )

		fw.server:broadcast_chat_message( "Server is shutting down, requested by " .. username .. ".", "Status", "Server" )
		--fw.server:shutdown()
	end
end

function on_rehash_command( args, sender )
	fw.server:broadcast_chat_message( "Scripts rehashed.", "Status", "Server" )
end

function on_help_command( args, sender )
	fw.server:broadcast_chat_message( "Commands: /rehash /shutdown", "Status", "Server" )
end

fw.event:hook_command( "shutdown", on_shutdown_command )
fw.event:hook_command( "rehash", on_rehash_command )
fw.event:hook_command( "help", on_help_command )
