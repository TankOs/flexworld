--
-- Initialization stuff for new players.
--

function on_connect( client_id )
	local entity_id = flex.server:get_client_entity_id( client_id )
	local username = flex.server:get_client_username( client_id )

	-- Create a brand new backpack.
	flex.world:create_attached_entity( "fw.containers/backpack", entity_id, "inventory" )
	flex.server:broadcast_chat_message( username .. ", here's a new backpack for you!", "Status", "Server" )
end

flex.event:hook_system_event( flex.Event.System.CONNECT, on_connect )
