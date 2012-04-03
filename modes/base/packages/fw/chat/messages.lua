function on_connect_system_event( client )
	print( "Client " .. client .. " connected." )
end

flex.event:hook_system_event( flex.Event.System.CONNECT, on_connect_system_event )
