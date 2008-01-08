
//AsyncRequest turnOnForFiles := method(

File readToBufferLength := method(buffer, size,
	request := AsyncRequest clone setDescriptor(self descriptor)
	event := ReadEvent clone setDescriptorId(self descriptor)

	pos := self position
	request read(pos, size)

	while(request isDone not, event waitOnOrExcept(1))

	errormsg := request error
	if(errormsg, Exception raise("AsyncRequest error: " .. errormsg); return nil)

	request copyBufferTo(buffer)
	self setPosition(pos + buffer size)
	buffer
)

File readBufferOfLength := method(size,
	buffer := Sequence clone
	readToBufferLength(buffer, size)
	buffer
)

File write := method(buffer,
	request := AsyncRequest clone setDescriptor(descriptor)
	event := WriteEvent clone setDescriptorId(descriptor)

	pos := self position
	request write(pos, buffer, 0, buffer size)

	while(request isDone not, event waitOnOrExcept(1))

	errormsg := request error
	if(errormsg, Exception raise("AsyncRequest error: " .. errormsg); return nil)
	//request sync

	setPosition(pos + buffer size)
	self
)
//)
