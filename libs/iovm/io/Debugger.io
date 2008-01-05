Debugger do(
	docDescription("To start debugging a coroutine, call
	<pre>
	Coroutine currentCoroutine setMessageDebugging(true)
	</pre>
	Then each message sent within that coroutine will cause the Debugger vmWillSendMessage slot to be activated and the Debugger slots: messageCoroutine, messageSelf, messageLocals, and message will be set with the values related to the current message send. You can override vmWillSendMessage to implement your own debugging mechanisms.")

	start := method(
		loop(
			self vmWillSendMessage(self message name)
			messageCoroutine resume
		)
	)

	docSlot("vmWillSendMessage", "Override this method to implement your own debugging mechanisms.")

	vmWillSendMessage := method(
		writeln("Debugger vmWillSendMessage(", self message name, ")")
	)

	debuggerCoroutine := coroFor(start)
	yield
)


