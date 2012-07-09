function on_eval_command( args, sender )
	if #args < 1 then
		fw.server:broadcast_chat_message( "Usage: /eval LUA_CODE", "Status", "Server" )
		return
	end

	f = loadstring( table.concat( args, " " ) )
	f()
end

fw.event:hook_command( "eval", on_eval_command )
