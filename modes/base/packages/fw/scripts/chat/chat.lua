--
-- Chat.
--

function on_chat_system_event( message, channel, sender )
	local username = fw.server:get_client_username( sender )
	print( "[" .. channel .. "] <" .. username .. "> " .. message )

	-- Do not allow chatting to the "Status" channel.
	if channel == "Status" then
		-- TODO Replace by 'send_chat_message'.
		fw.server:broadcast_chat_message( "You're not allowed to chat to this channel.", channel, "Server" )
	else
		fw.server:broadcast_chat_message( message, channel, username )
	end
end

fw.event:hook_event( fw.Event.CHAT, on_chat_system_event )
