//metadoc Beanstalk Aleksey Yeschenko, 2009
//metadoc Beanstalk license BSD revised
//metadoc Beanstalk category Messaging
//metadoc Beanstalk credits Aleksey Yeschenko, 2009. Default arguments and some other things taken from beanstalk-client-ruby by Keith Rarick.
/*metadoc Beanstalk description
beanstalkd is a fast, distributed, in-memory workqueue service. See http://xph.us/software/beanstalkd/
<br />An example from http://xph.us/software/beanstalkd/:
<br />First, have one process put a job into the queue:
<pre>
producer := Beanstalk clone connect("127.0.0.1:11300")
producer put("hello")
</pre>

Then start another process to take jobs out of the queue and run them:
<pre>
worker := Beanstalk clone connect("127.0.0.1:11300")
loop(
	job := worker reserve
	job body println # prints "hello"
	job delete
)
</pre>
See Beanstalk.io code and protocol description (http://github.com/kr/beanstalkd/tree/master/doc/protocol.txt) for details.
Both are short and easy to read.

<br />Stat commands depend on <a href="http://github.com/why/syck/tree/a4f241be5d247853aea6127d02dbdedd8a1dd477/ext/io">YAML</a>.
*/
Beanstalk := Object clone do(

	//doc Beanstalk connect(address) Connects to a beanstalk server. address is a "host:port" string, e.g., "127.0.0.1:11300"
	connect := method(addr,
		host := addr split(":") first
		port := addr split(":") second asNumber
		self socket := Socket clone setHost(host) setPort(port) connect
		self
	)

	disconnect := method(
		socket close
		self
	)

	# Producer Commands

	/*doc Beanstalk put(body, pri, delay, ttr)
	Inserts a job into the queue.
	<br />pri - priority, an integer < 2**32. Jobs with smaller priority values will be
	scheduled before jobs with larger priorities. The most urgent priority is 0;
	the least urgent priority is 4294967295.
	<br />delay - an integer number of seconds to wait before putting the job in
	the ready queue. The job will be in the "delayed" state during this time.
	<br />ttr - time to run, an integer number of seconds to allow a worker to run this job.
	*/
	put := method(body, pri, delay, ttr,
		body = body asString
		if(pri == nil, pri = 65536)
		if(delay == nil, delay = 0)
		if(ttr == nil, ttr = 120)

		cmd := "put #{pri} #{delay} #{ttr} #{body size}\r\n#{body}" interpolate
		command(cmd, list("INSERTED", "BURIED")) at(1) asNumber
	)

	use := method(tube,
		command("use #{tube}" interpolate, "USING") at(1)
	)

	# Worker Commands

	//doc Beanstalk reserve(timeout) Returns and reserves a job (waits until one becomes available if necessary)
	reserve := method(timeout,
		cmd := if(timeout == nil, "reserve", "reserve-with-timeout #{timeout}" interpolate)
		readJob(command(cmd, "RESERVED"))
	)

	reserveWithTimeout := method(timeout,
		reserve(timeout)
	)

	//doc Beanstalk delete(id) Removes a job with a given id from the server, entirely
	delete := method(id,
		command("delete #{id}" interpolate, "DELETED")
		self
	)

	//doc Beanstalk release(id) Puts a reserved job back into the ready queue
	release := method(id, pri, delay,
		if(pri == nil, pri = 65536)
		if(delay == nil, delay = 0)
		command("release #{id} #{pri} #{delay}" interpolate, "RELEASED")
		self
	)

	//doc Beanstalk bury(id, pri) Puts a job into the "buried" state
	bury := method(id, pri,
		if(pri == nil, pri = 65536)
		command("bury #{id} #{pri}" interpolate, "BURIED")
		self
	)

	//doc Beanstalk touch(id) Allows a worker to request more time to work on a job.
	touch := method(id,
		command("touch #{id}" interpolate, "TOUCHED")
		self
	)

	watch := method(tube,
		command("watch #{tube}" interpolate, "WATCHING") at(1) asNumber
	)

	ignore := method(tube,
		command("ignore #{tube}" interpolate, "WATCHING") at(1) asNumber
	)

	# Other Commands
	peek := method(id,
		peekGeneric("peek #{id}" interpolate)
	)

	peekReady := method(
		peekGeneric("peek-ready")
	)

	peekDelayed := method(
		peekGeneric("peek-delayed")
	)

	peekBuried := method(
		peekGeneric("peek-buried")
	)

	kick := method(bound,
		command("kick #{bound}" interpolate, "KICKED") at(1) asNumber
	)

	statsJob := method(id,
		readYAML("stats-job #{id}" interpolate)
	)

	statsTube := method(tube,
		readYAML("stats-tube #{tube}" interpolate)
	)

	stats := method(
		readYAML("stats")
	)

	listTubes := method(
		readYAML("list-tubes")
	)

	listTubeUsed := method(
		command("list-tube-used", "USING") at(1)
	)

	listTubesWatched := method(
		readYAML("list-tubes-watched")
	)

	# Internals
	command := method(cmd, expected,
		socket streamWrite(cmd .. "\r\n")
		expected = list(expected) flatten
		response := socket readUntilSeq("\r\n") split
		if(response containsAny(expected),
			response,
			Exception raise(errorWithMessage(response at(0)))
		)
	)

	peekGeneric := method(cmd,
		readJob(command(cmd, "FOUND")) setReserved(false)
	)

	readJob := method(response,
		id   := response at(1) asNumber
		size := response at(2) asNumber # excluding \r\n
		body := socket readBytes(size + 2) inclusiveSlice(0, size - 1)
		BeanstalkJob with(id, body, self)
	)

	readYAML := method(cmd,
		size := command(cmd, "OK") at(1) asNumber
		data := socket readBytes(size + 2) inclusiveSlice(0, size - 1)
		YAML load(data)
	)

	# Common Errors
	OutOfMemoryError    := Error with("OUT_OF_MEMORY")
	InternalError       := Error with("INTERNAL_ERROR")
	DrainingError       := Error with("DRAINING")
	BadFormatError      := Error with("BAD_FORMAT")
	UnknownCommandError := Error with("UNKNOWN_COMMAND")

	# put Errors
	ExpectedCRLFError   := Error with("EXPECTED_CRLF")
	JobTooBigError      := Error with("JOB_TOO_BIG")

	# reserve and reserve-with-timeout Errors
	DeadlineSoonError   := Error with("DEADLINE_SOON")
	TimedOutError       := Error with("TIMED_OUT")

	# delete, release, bury and touch Errors
	NotFoundError       := Error with("NOT_FOUND")

	# ignore Errors
	NotIgnoredError     := Error with("NOT_IGNORED")

	# peek, peek-ready, peek-delayed and peek-buried can respond
	# with NOT_FOUND, which is defined already
	# same with stats-job and stats-tube

	allErrors := method(
		Beanstalk slotNames select(endsWithSeq("Error")) map(name, self getSlot(name))
	)

	errorWithMessage := method(msg,
		allErrors detect(message == msg)
	)

)

//metadoc BeanstalkJob module Beanstalk
//metadoc BeanstalkJob description Represents a job from the queue
BeanstalkJob := Object clone do(

	id ::= nil
	body ::= nil
	connection ::= nil
	reserved ::= nil

	with := method(id, body, connection, reserved,
		j := self clone
		j setId(id) setBody(body) setConnection(connection)
		if(reserved == nil, reserved = true)
		j setReserved(reserved)
		j
	)

	//doc BeanstalkJob delete See Beanstalk delete
	delete := method(
		connection delete(id)
		reserved = false
		self
	)

	//doc BeanstalkJob release(pri, delay) See Beanstalk release
	release := method(pri, delay,
		if(reserved, connection release(id, pri, delay))
		reserved = false
		self
	)

	//doc BeanstalkJob bury(pri) See Beanstalk bury
	bury := method(pri,
		if(reserved, connection bury(id, pri))
		reserved = false
		self
	)

	//doc BeanstalkJob touch See Beanstalk touch
	touch := method(
		if(reserved, connection touch(id))
		self
	)

	stats := method(
		connection statsJob(id)
	)

)
