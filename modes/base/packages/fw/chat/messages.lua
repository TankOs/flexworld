function on_connect_system_event( client )
	print( "Client " .. client .. " connected." )

	local username = flex.server:get_client_username( client )

	flex.server:broadcast_chat_message( username .. " joined the game.", "Status", "" )
end

flex.event:hook_system_event( flex.Event.System.CONNECT, on_connect_system_event )
