
LoginPanel := Panel clone do(
    init := method(
		resend
		
		setTitle("Login")
		resizeHeightTo(150)
		
		self usernameLabel := Label clone
		usernameLabel position set(30, height - 55)
		usernameLabel resizeHeight := 011
		usernameLabel setTitle("Username:")
		addSubview(usernameLabel)
	
		self usernameTextField := TextField clone do(
			resizeWidth  = 101
			resizeHeight = 011
			setString("")
		)
		
		usernameTextField position set(100, height - 60)
		usernameTextField setWidth(180)
		addSubview(usernameTextField)
		
	
		self text := Label clone
		text position set(35, height - 85)
		text resizeHeight := 011
		text setTitle("Password:")
		addSubview(text)
		
		self passwordTextField := TextField clone do(
			resizeWidth  = 101
			resizeHeight = 011
			setString("")
		)
		
		passwordTextField position set(100, height - 90)
		passwordTextField  setWidth(180)
		addSubview(passwordTextField)
	
		passwordTextField renderString := method(
			s := Sequence clone
			string foreach(i, v, s appendSeq("*"))
			s
		)
	
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
		
		// -----------------------------
		action := Action clone
		action setActionTarget(passwordTextField)
		action setAction("makeFirstResponder")
		usernameTextField forKeyAddAction("\t", action)
		
		action := Action clone
		action setActionTarget(usernameTextField)
		action setAction("makeFirstResponder")
		passwordTextField forKeyAddAction("\t", action)
		
		action := Action clone
		action setActionTarget(okButton)
		action setAction("doAction")
		passwordTextField forKeyAddAction("\n", action)
		
		action := Action clone
		action setActionTarget(okButton)
		action setAction("doAction")
		passwordTextField forKeyAddAction("\r", action)
    )
    
    okAction := method(
		actionTarget perform(action, self)
		close
    )

    username := method(usernameTextField string asString)
    password := method(passwordTextField string asString)
    
    open := method(
		okButton sizeToTitle 
		okButton position set(width - okButton width - 15, 15)
		cancelButton sizeToTitle 
		cancelButton placeLeftOf(okButton)
		resend
    )
)
