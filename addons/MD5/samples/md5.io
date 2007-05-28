#!/usr/bin/env io

File md5String := method(
	digest := MD5 clone
	buffer := Buffer clone
	
	openForReading
	while(isAtEnd == nil,
		readToBufferLength(buffer, 65536)
		digest appendSeq(buffer)
	)
	close
	digest md5String
)

if (?args == nil or args size == 0,
	write("requires a file name as an argument\n")
	exit
)

f := File clone setPath(args at(0))
m1 := f md5String
m2 := f asBuffer md5String
write("md5 = ", m1, "\n")
write("md5 = ", m2, " \n")
write(if(m1 == m2, "PASSED", "FAILED"), " TEST\n")


