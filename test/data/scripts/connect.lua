function on_connect_system_event( client )
	assert( client == 1337 )
end

flex.event:hook_system_event( flex.Event.System.CONNECT, on_connect_system_event )
