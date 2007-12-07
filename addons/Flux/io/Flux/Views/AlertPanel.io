
AlertPanel := Panel clone do(
	init := method(
		resend
		
		//resizeTo(300,150)
		self header := Label clone
		header position set(30, height - 60)
		header resizeHeight := 011
		
		self text := Label clone
		text position set(30, height - 80)
		text resizeHeight := 011
		addSubview(text)
		
		self okButton := Button clone
		okButton setTitle("OK")
		okButton position set(width - okButton width - 15, 15)
		okButton resizeWidth = 011
		okButton setActionTarget(self)
		okButton setAction("okAction")
		addSubview(okButton)
		
		self cancelButton := Button clone
		cancelButton setTitle("Cancel")
		cancelButton placeLeftOf(okButton)
		cancelButton resizeWidth = 011
		cancelButton setActionTarget(self)
		cancelButton setAction("close")
		addSubview(cancelButton)
	)
	
	setMessage := method(m,
		text setTitle(m)
	)
	
	okAction := method(
		actionTarget perform(action, self)
		close
	)
	
	cancelAction := method(
		actionTarget perform(action, self)
		close
	)
)
