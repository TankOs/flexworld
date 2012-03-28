--
-- Tests for class events.
--

-- USE event.
function on_a_use( entity, actor )
	flex.test:set_value( "a_use", entity .. "," .. actor )
end

function on_b_use( entity, actor )
	flex.test:set_value( "b_use", entity .. "," .. actor )
end

flex.event:hook_class_event( flex.Event.Class.USE, "class/a", on_a_use )
flex.event:hook_class_event( flex.Event.Class.USE, "class/b", on_b_use )
