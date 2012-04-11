function faulty( block, next_block, primary, actor, client_id )
	assert( false )
end

function handler( block, next_block, primary, actor, client_id )
	assert( block[1] == 1 and block[2] == 2 and block[3] == 3 )
	assert( next_block[1] == 4 and next_block[2] == 5 and next_block[3] == 6 )
	assert( primary == true )
	assert( actor == 100 )
	assert( client_id == 1337 )
end
