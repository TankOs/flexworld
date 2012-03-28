--
-- Script for testing command hooks.
--

function on_hello_world_command( args )
	assert( #args == 2 )
	assert( args[1] == "hello" )
	assert( args[2] == "world" )

	flex.test:set_value( "hello_world", args[1] .. " " .. args[2] )
end

flex.event:hook_command( "hello_world", on_hello_world_command )
