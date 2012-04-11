--
-- Tests for class events.
--

-- USE event.
function on_a_use( entity, actor )
	flex.test:set_value( "a_use", entity .. "," .. actor )
end

function on_b_use( entity, actor )
	flex.test:set_value( "b_use", entity .. "," .. actor )
end

-- BLOCK_ACTION event
function on_block_action_event( block, next_block, primary, actor, client_id )
	assert( block[1] == 1 and block[2] == 2 and block[3] == 3 )
	assert( next_block[1] == 4 and next_block[2] == 5 and next_block[3] == 6 )
	assert( primary == true )
	assert( actor == 100 )
	assert( client_id == 1337 )

	flex.test:set_value( "block_action", "called" )
end

flex.event:hook_class_event( flex.Event.Class.USE, "class/a", on_a_use )
flex.event:hook_class_event( flex.Event.Class.USE, "class/b", on_b_use )
flex.event:hook_class_event( flex.Event.Class.BLOCK_ACTION, "class/a", on_block_action_event )
