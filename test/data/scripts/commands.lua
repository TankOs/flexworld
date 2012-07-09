--
-- Script for testing command hooks.
--

function on_hello_world_command( args, sender )
	assert( #args == 2 )
	assert( args[1] == "hellä" )
	assert( args[2] == "hellö" )
	assert( sender == 1337 )

	-- Check UTF-8.
	assert( #args[1] == 6 )
	assert( #args[2] == 6 )

	fw.test:set_value( "hello_world", args[1] .. " " .. args[2] )
end

fw.event:hook_command( "hello_world", on_hello_world_command )
