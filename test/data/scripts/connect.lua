function on_connect_system_event( client )
	assert( client == 1337 )
end

flex.event:hook_event( flex.Event.CONNECT, on_connect_system_event )
