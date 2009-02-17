
File sha1String := method(
	digest := SHA1 clone
	buffer := Sequence clone

	openForReading
	while(isAtEnd not,
		readToBufferLength(buffer, 65536)
		digest appendSeq(buffer)
	)
	close
	digest sha1String
)

Sequence sha1String := method(
	SHA1 clone appendSeq(self) sha1String
)

Sequence sha1 := method(
	SHA1 clone appendSeq(self) sha1
)

Sequence hashString := Sequence getSlot("sha1String")
Sequence hash := Sequence getSlot("sha1")