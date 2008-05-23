

Responder := GLObject clone do(
    nextResponder ::=nil
        
    actionTarget ::= nil
    action ::= nil
    
    doAction := method(
		if(actionTarget and action, 
			if (actionTarget getSlot(action)) then(
				actionTarget perform(action, self)
			) else (
				write("Warning: missing action '", action, "' on actionTarget\n")
			)
		)
    )
)
