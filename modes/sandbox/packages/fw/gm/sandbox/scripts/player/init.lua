--
-- Initialization stuff for new players.
--

function on_connect( client_id )
	local entity_id = fw.server:get_client_entity_id( client_id )
	local backpack_id = fw.world:create_entity( "fw.items.containers/backpack", entity_id, "inventory" )

	--fw.world:create_entity( "fw.animals/horse", backpack_id )
	fw.world:create_entity( "fw.items.tools/shovel", backpack_id )
	print( "OK" )
end

fw.event:hook_event( fw.Event.CONNECT, on_connect )
