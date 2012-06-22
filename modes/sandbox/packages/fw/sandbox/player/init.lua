--
-- Initialization stuff for new players.
--

function on_connect( client_id )
	local entity_id = flex.server:get_client_entity_id( client_id )
	local backpack_id = flex.world:create_entity( "fw.containers/backpack", entity_id, "inventory" )

	--flex.world:create_entity( "fw.animals/horse", backpack_id )
	--flex.world:create_entity( "fw.tools/shovel", backpack_id )
end

flex.event:hook_system_event( flex.Event.System.CONNECT, on_connect )
