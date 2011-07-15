ObjcBridge debugOn


Test := Object clone

NSNotificationCenter defaultCenter addObserver:selector:name:object:(Test, "test:", "test123", nil)

Test test: := method(
	"got notification" println
)

NSNotificationCenter defaultCenter postNotificationName:object:("test123", nil)