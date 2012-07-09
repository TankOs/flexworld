function on_connect_system_event( client )
	local username = fw.server:get_client_username( client )

	print( "Client " .. username .. " connected (total users now: " .. fw.server:get_num_connected_clients() .. ")" )
	fw.server:broadcast_chat_message( username .. " joined the game.", "Status", "Server" )
end

fw.event:hook_event( fw.Event.CONNECT, on_connect_system_event )
