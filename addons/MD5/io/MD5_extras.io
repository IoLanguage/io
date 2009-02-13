
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

Sequence do(
	md5String := method(MD5 clone appendSeq(self) md5String)
	md5 := method(MD5 clone appendSeq(self) md5)
)


//Sequence hashString := Sequence getSlot("md5String")
//Sequence hash := Sequence getSlot("md5")