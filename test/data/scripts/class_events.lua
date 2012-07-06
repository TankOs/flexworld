--
-- Tests for class events.
--

-- USE event.
function on_actor_use( entity, actor, client_id )
	if entity == 100 then
		assert( client_id == 111 )
	elseif entity == 200 then
		assert( client_id == 222 )
	else
		assert( "Invalid entity ID." )
	end

	flex.test:set_value( "use", entity .. "," .. actor .. "," .. client_id )
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

flex.event:hook_event( flex.Event.USE, on_actor_use )
flex.event:hook_event( flex.Event.BLOCK_ACTION, on_block_action_event )
