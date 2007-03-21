
Action := Responder clone do(
    newSlot("actionTarget")
    newSlot("actionMessage")
    
    action := nil
    setAction := method(actionName,
		action = actionName
		setActionMessage(Message clone setName(actionName))
    )
    
    newSlot("argument")
    
    send := method(
		if (actionTarget and action,
			actionTarget doMessage(actionMessage) // faster than perform
		)
    )
)



