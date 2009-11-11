Chart := Object clone do(
	appendProto(Cairo)
	data ::= list(list(0, .1), list(.1, 0), list(.2, .1), list(.3, .5), list(.4, .1), list(1, 0))
	filename ::= "chart.png"
	scale := vector(600, 600)
	draw := method(
		surface := ImageSurface create(FORMAT_ARGB32, scale x, scale y)
		context := Context create(surface) 
		context scale(scale x, scale y)
		context setLineWidth(0.04)
		
		//context selectFontFace("Sans", FONT_SLANT_NORMAL, FONT_WEIGHT_BOLD)
		
		context setSourceRGBA(0, 0, 0, 1)
		context setLineWidth(0.01)
		context setLineJoin(LINE_JOIN_ROUND)
		context setLineCap(LINE_CAP_ROUND)
		
		
		context moveTo(data at(0) at(0), data at(0) at(1))
		data foreach(i, p,
			//context showText(i asString)
			context lineTo(p at(0), p at(1))
		)
		context stroke
		
		context setSourceRGBA(0, 0, 0, .1)
		
		context setLineWidth(0.01)
		data foreach(i, p,
			x := p at(0)
			context moveTo(x, 0)
			context lineTo(x, 1)
			context stroke
		)
		
		surface writeToPNG(filename)
	)
)

Chart draw
