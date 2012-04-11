--
-- Block tools for players.
--

function on_build_class_block_action( block, next_block, primary, actor, client_id )
	local planet = flex.world:get_entity_planet( actor )
	local reply_channel = "@" .. planet
	local username = flex.server:get_username( client_id )

	if primary == false then
		-- Secondary action destroys a block.
		flex.world:destroy_block( block, planet )
		flex.server:broadcast_chat_message( username .. " destroyed a block @ " .. block[1] .. ", " .. block[2] .. ", " .. block[3] .. ".", reply_channel, "Server" )
	end
end

flex.event:hook_class_event( flex.Event.Class.BLOCK_ACTION, "fw.sandbox.player/dwarf_male", on_build_class_block_action )
