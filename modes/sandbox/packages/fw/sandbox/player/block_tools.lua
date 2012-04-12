--
-- Block tools for players.
--

next_class_id = "fw.base.nature/stone"

function on_build_class_block_action( block, next_block, primary, actor, client_id )
	local planet = "construct" -- TODO
	local reply_channel = "@" .. planet
	local username = flex.server:get_client_username( client_id )

	if primary == false then
		-- Secondary action destroys a block.
		flex.world:destroy_block( block, planet )
		--flex.server:broadcast_chat_message( username .. " destroyed a block @ " .. block[1] .. ", " .. block[2] .. ", " .. block[3] .. ".", reply_channel, "Server" )
	else
		-- Primary action places a block.
		flex.world:set_block( next_block, planet, next_class_id )
	end
end

function on_set_next_command( args, client_id )
	if #args < 1 then
		flex.server:broadcast_chat_message( "Usage: /set_next CLASS_ID" )
		return
	end

	next_class_id = args[1]
end

flex.event:hook_class_event( flex.Event.Class.BLOCK_ACTION, "fw.sandbox.player/dwarf_male", on_build_class_block_action )
flex.event:hook_command( "set_next", on_set_next_command )
