
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

SHA1 hashString := SHA1 getSlot("sha1String")
SHA1 hash := SHA1 getSlot("sha1")