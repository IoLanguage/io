#!/usr/bin/env io

File md5String := method(
	digest := MD5 clone
	buffer := Sequence clone
	
	openForReading
	while(isAtEnd == nil,
		readToBufferLength(buffer, 65536)
		digest appendSeq(buffer)
	)
	close
	digest md5String
)

if (System ?args == nil or System args size == 0,
	write("requires a file name as an argument\n")
	exit
)

f := File clone setPath(System args at(1))
m1 := f md5String
m2 := f asBuffer md5String
write("md5 = ", m1, "\n")
write("md5 = ", m2, " \n")
write(if(m1 == m2, "PASSED", "FAILED"), " TEST\n")


