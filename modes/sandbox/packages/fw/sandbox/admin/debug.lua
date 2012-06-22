function spawn_entity_at_client( class_id, client_id )
	local entity_id = flex.server:get_client_entity_id( client_id )
	local pos, planet_id = flex.world:get_entity_position( entity_id )

	flex.world:create_entity( class_id, pos, planet_id )

	flex.server:broadcast_chat_message( "Spawned " .. class_id .. " @ " .. planet_id .. "/" .. pos[1] .. ", " .. pos[2] .. ", " .. pos[3], "DEBUG", "debug.lua" )
end

function on_dwarf_command( args, client_id )
	spawn_entity_at_client( "fw.base.human/dwarf_male", client_id )
end

function on_horse_command( args, client_id )
	spawn_entity_at_client( "fw.animals/horse", client_id )
end

flex.event:hook_command( "dwarf", on_dwarf_command )
flex.event:hook_command( "horse", on_horse_command )
