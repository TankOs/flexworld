--
-- Several tools for modifying blocks.
--

-- TODO Error messages.
function on_dblock_command( args, sender )
	local username = flex.server:get_client_username( sender )

	if #args ~= 3 then
		return
	end

	x = tonumber( args[1] )
	y = tonumber( args[2] )
	z = tonumber( args[3] )

	if x < 0 or y < 0 or z < 0 then
		return
	end

	local planet = "construct" -- TODO Replace by real planet.

	flex.world:destroy_block( x, y, z, planet )
	flex.server:broadcast_chat_message( username .. " destroyed block @ " .. x .. ", " .. y .. ", " .. z .. ".", "@" .. planet, "Server" )
end

flex.event:hook_command( "dblock", on_dblock_command )
