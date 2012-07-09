--
-- Test script for chat events.
--

function on_chat_event( msg, channel, sender )
	assert( msg == "Hellö" )
	assert( channel == "Chännel" )
	assert( sender == 1337 )
end

fw.event:hook_event( fw.Event.CHAT, on_chat_event )
