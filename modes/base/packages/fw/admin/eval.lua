function on_eval_command( args, sender )
	if #args < 1 then
		flex.server:broadcast_chat_message( "Usage: /eval LUA_CODE", "Status", "Server" )
		return
	end

	f = loadstring( table.concat( args, " " ) )
	f()
end

flex.event:hook_command( "eval", on_eval_command )
