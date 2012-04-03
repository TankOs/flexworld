function on_connect_system_event( client )
	local username = flex.server:get_client_username( client )

	print( "Client " .. username .. " connected (total users now: " .. flex.server:get_num_connected_clients() .. ")" )
	flex.server:broadcast_chat_message( username .. " joined the game.", "Status", "Server" )
end

flex.event:hook_system_event( flex.Event.System.CONNECT, on_connect_system_event )
