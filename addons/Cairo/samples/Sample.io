if (Cairo HAS_PNG_FUNCTIONS not,
	Exception raise("Can't run sample because cairo was compiled without PNG support")
)

Sample := Object clone
Sample context := method(
	appendProto(Cairo)
	
	surface := ImageSurface create(FORMAT_ARGB32, 256, 256)
	Context create(surface) do(
		appendProto(Cairo)
	
		pi := Number constants pi

		_do := getSlot("do")
		do := method(
			scale(256, 256)
			setLineWidth(0.04)
		
			m := Message clone setName("_do") appendArg(call argAt(0))
			self perform(m)
		
			filename := call message label asMutable removeSuffix(".io") appendSeq(".png")
			getTarget writeToPNG(filename)
		)
	)
)
