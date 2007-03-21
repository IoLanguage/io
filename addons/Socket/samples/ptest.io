

List myCoroDo := method(
	coro := Coroutine clone
	//writeln("coroDo sender = ", call sender slotNames )
	coro setRunTarget(call sender) 
	coro setRunLocals(call sender)
	coro setRunMessage(call argAt(0)) 
	Coroutine yieldingCoros atInsert(0, Scheduler currentCoroutine)
	coro run
)
	
List pforeach := method(
	indexName := call message argAt(0) name
	valueName := call message argAt(1) name
	maxRunning := call evalArgAt(3)
	running := 0
	
	writeln("this coro ", Scheduler currentCoroutine uniqueId)
	
	self foreach(i, v, 
		context := Object clone
		context setSlot(indexName, i)
		context setSlot(valueName, getSlot("v"))
		context setProto(call sender)
		
		coro := myCoroDo(running = running + 1; try(context doMessage(call argAt(2)); writeln("done1")); writeln("done2"); running = running - 1)
		writeln("coro ", coro uniqueId)
		while(running == maxRunning, yield) // optimize to non busy wait later
		writeln("--- ", running)
	)
	//while(running > 0, writeln("--> ", running); yield)
	while(running > 0, 
		writeln("--> ", running, " ", Coroutine yieldingCoros size, " ", Coroutine yieldingCoros map(uniqueId)); 
		wait(1)
	)
	self
)

URL

url := "http://www.yahoo.com/"
urls := List clone
for(i, 1, 6, urls append(url asURL))

//list(1,2,3,4,5,6,7,8,9) pforeach(i, v, yield; (v + 1) println, 3)

Date secondsToRun(urls pforeach(i, v, v fetch size println, 3)) println
//Date secondsToRun(urls pforeach(i, v, wait(1), 3)) println

/*
1 0:06.25 
2 0:06.23
3 0:05.79
10 0:03.86
*/