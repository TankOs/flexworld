--
-- Test script for chat events.
--

function on_chat_event( msg, channel, sender )
	assert( msg == "Hellö" )
	assert( channel == "Chännel" )
	assert( sender == 1337 )
end

flex.event:hook_system_event( flex.Event.System.CHAT, on_chat_event )
