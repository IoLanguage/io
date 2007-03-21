

Responder := GLObject clone do(
    newSlot("nextResponder")
    
    // --------------------------------------
    
    newSlot("actionTarget")
    newSlot("action")
    
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
