NotificationListener := Object clone do(
	//metadoc NotificationListener category Core
	
	newSlot("sender")
	//doc NotificationListener sender Returns the sender that the listener wants to receive messages from.
	//doc NotificationListener setSender(anObject) Sets the sender that the listener wants to receive messages from. Returns self.
	
	newSlot("name")
	//doc NotificationListener name Returns the message name that the listener wants to receive messages for.
	//doc NotificationListener setName(aSeq) Sets the message name that the listener wants to receive messages for. Returns self.
	
	newSlot("target")
	//doc NotificationListener target Returns the target that the receiver sends matching messages to.
	//doc NotificationListener setTarget(anObject) Sets the target that the receiver sends matching messages to. Returns self.
	
	newSlot("action", "notification")
	//doc NotificationListener action Returns the name of the message that is sent to the target when a matching notification is found.
	//doc NotificationListener setAction(messageName) Sets the name of the message that is sent to the target when a matching notification is found. Returns self.
	
	handleNotification := method(n,
/*doc NotificationListener handleNotification(aNotification) 
Checks to see if aNotification matches the listener criteria and if so, sends it to the target. Implementation:
<pre>
if((name == nil or name == n name) and(sender == nil or sender == n sender),
	target perform(action, n)
)
</pre>
*/
		if((name == nil or name == n name) and(sender == nil or sender == n sender),
			target perform(action, n)
		)
	)
	start := method(
		//doc NotificationListener start Adds the receiver to the NotificationCenter set of listeners. Returns self.
		NotificationCenter addListener(self); self
	)
	
	stop := method(
		//doc NotificationListener start Removes the receiver to the NotificationCenter set of listeners. Returns self.
		NotificationCenter removeListener(self); self
	)
)

Notification := Object clone do(
	//metadoc Notification category Core
	
	newSlot("sender")
	//doc Notification sender Returns the sender of the notification.
	//doc Notification setSender(anObject) Sets the sender of the Notification. Returns self.
		
	newSlot("name")
	//doc Notification name Returns the name of the notification.
	//doc Notification setName(anObject) Sets the name of the Notification. Returns self.
	
	post := method(
		//doc Notification post Posts the notification to the NotificationCenter.
		NotificationCenter postNotification(self)
	)
)

NotificationCenter := Object clone do(
	//metadoc NotificationCenter category Core
/*metadoc NotificationCenter description

An NotificationCenter similar to the one found in Apple's FoundationKit.
<p>
Example use:
<pre>
// in listener
listener := NotificationListener clone setTarget(self) setName("FeedDownloadedFile") start

// in sender
Notification clone setSender(self) setName("FeedDownloadedFile") post

// note: notifications can also be reused, and any extra data can be added in it's slots

// to stop listening
listener stop
</pre>
*/

	listeners := List clone
	//doc NotificationCenter listeners Returns list of NotificationListeners.
	
	addListener := method(e, listeners appendIfAbsent(e); self)
	//doc NotificationCenter addListener(aNotificationListener) Adds aNotificationListener to listeners list. Returns self.
	
	removeListener := method(e, listeners remove(e); self)
	//doc NotificationCenter addListener(aNotificationListener) Removes aNotificationListener from listeners list. Returns self.

	postNotification := method(n, 
/*doc NotificationCenter postNotification(aNotification) 
Sends aNotification to be handled by listeners. Returns self.  
Implementation:
<pre>
listeners foreach(handleNotification(n))
</pre>
*/
		listeners foreach(handleNotification(n)); self
	)
)

