--
-- Several tools for modifying blocks.
--

-- TODO Error messages.
function on_dblock_command( args, sender )
	local username = flex.server:get_client_username( sender )
	local planet = "construct" -- TODO Replace by real planet.
	local reply_channel = "@" .. planet

	if #args ~= 3 then
		flex.server:broadcast_chat_message( "Usage: /dblock X Y Z", reply_channel, "Server" )
		return
	end

	x = tonumber( args[1] )
	y = tonumber( args[2] )
	z = tonumber( args[3] )

	if x < 0 or y < 0 or z < 0 then
		return
	end

	flex.world:destroy_block( {x, y, z}, planet )
	flex.server:broadcast_chat_message( username .. " destroyed block @ " .. x .. ", " .. y .. ", " .. z .. ".", reply_channel, "Server" )
end

function on_dblockrange_command( args, sender )
	local username = flex.server:get_client_username( sender )
	local planet = "construct" -- TODO Replace by real planet.
	local reply_channel = "@" .. planet

	if #args ~= 6 then
		flex.server:broadcast_chat_message( "Usage: /dblockrange X0 Y0 Z0 X1 Y1 Z1", reply_channel, "Server" )
		return
	end

	x0 = tonumber( args[1] )
	y0 = tonumber( args[2] )
	z0 = tonumber( args[3] )
	x1 = tonumber( args[4] )
	y1 = tonumber( args[5] )
	z1 = tonumber( args[6] )

	if x0 < 0 or y0 < 0 or z0 < 0 or x1 < 0 or y1 < 0 or z1 < 0 then
		return
	end

	if x0 > x1 or y0 > y1 or z0 > z1 then
		return
	end

	for rx = x0, x1 do
		for ry = y0, y1 do
			for rz = z0, z1 do
				flex.world:destroy_block( {rx, ry, rz}, planet )
			end
		end
	end

	flex.server:broadcast_chat_message( username .. " destroyed block range.", reply_channel, "Server" )
end

flex.event:hook_command( "dblock", on_dblock_command )
flex.event:hook_command( "dblockrange", on_dblockrange_command )
