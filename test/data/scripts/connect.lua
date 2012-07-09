function on_connect_system_event( client )
	assert( client == 1337 )
end

fw.event:hook_event( fw.Event.CONNECT, on_connect_system_event )
