--
-- Block tools for players.
--

function on_build_class_block_action( block, next_block, primary, actor, client_id )
	local planet = flex.world:get_entity_planet( actor )

	if primary == false then
		-- Secondary action destroys a block.
		--flex.world:destroy_block( block, 
	end
end

flex.event:hook_class_event( flex.Event.Class.BLOCK_ACTION, "fw.sandbox.player/dwarf_male", on_build_class_block_action )
