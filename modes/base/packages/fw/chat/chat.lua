--
-- Chat.
--

function on_chat_system_event( message, channel, sender )
	local username = flex.server:get_client_username( sender )
	print( "[" .. channel .. "] <" .. username .. "> " .. message )

	-- Do not allow chatting to the "Status" channel.
	if channel == "Status" then
		-- TODO Replace by 'send_chat_message'.
		flex.server:broadcast_chat_message( "You're not allowed to chat to this channel.", channel, "Server" )
	else
		flex.server:broadcast_chat_message( message, channel, username )
	end
end

flex.event:hook_system_event( flex.Event.System.CHAT, on_chat_system_event )
