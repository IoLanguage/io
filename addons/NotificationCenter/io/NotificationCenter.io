
NotificationListener := Object clone do(
	newSlot("sender")
	newSlot("name")
	newSlot("target")
	newSlot("action", "notification")
	handleNotification := method(n,
			if((name == nil or name == n name) and(object == nil or sender == n sender),
				target perform(action, n)
			)
	)
	start := method(NotificationCenter addListener(self); self)
	stop := method(NotificationCenter removeListener(self); self)
)

Notification := Object clone do(
	newSlot("sender")
	newSlot("name")
	post := method(NotificationCenter postNotification(self))
)

NotificationCenter := Object clone do(
	listeners := List clone
	addListener := method(e, listeners appendIfAbsent(e); self)
	removeListener := method(e, listeners remove(e); self)
	postNotification := method(n, listeners foreach(handleNotification(n)); self)
)

/*
Example use:

// in listener
listener := NotificationListener clone setTarget(self) setName("FeedDownloadedFile") start

// in sender
Notification clone setSender(self) setName("FeedDownloadedFile") post
// note: notifications can also be reused, and any extra data can be added in it's slots

// to stop listening
listener stop

*/