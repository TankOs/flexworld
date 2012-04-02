--
-- Chat.
--

function on_chat_system_event( message, channel, sender )
	print( "[" .. channel .. "] <" .. sender .. "> " .. message )
end

flex.event:hook_system_event( flex.Event.System.CHAT, on_chat_system_event )
