--
-- Block tools for players.
--

next_class_id = "fw.struct.simple/wooden_planks"

function on_build_class_block_action( block, next_block, primary, actor, client_id )
	local planet = "construct" -- TODO
	local reply_channel = "@" .. planet
	local username = fw.server:get_client_username( client_id )

	if primary == false then
		-- Secondary action destroys a block.
		fw.world:destroy_block( block, planet )
		--fw.server:broadcast_chat_message( username .. " destroyed a block @ " .. block[1] .. ", " .. block[2] .. ", " .. block[3] .. ".", reply_channel, "Server" )
	else
		-- Primary action places a block.
		fw.world:set_block( next_block, planet, next_class_id )
	end
end

function on_set_next_command( args, client_id )
	if #args < 1 then
		fw.server:broadcast_chat_message( "Usage: /set_next CLASS_ID", "Status", "Server" )
		return
	end

	next_class_id = args[1]
end

fw.event:hook_event( fw.Event.BLOCK_ACTION, on_build_class_block_action )
fw.event:hook_command( "set_next", on_set_next_command )
