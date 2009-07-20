#!/usr/bin/env io
/* 
   Based on code at 
   http://www.bluishcoder.co.nz/2009/06/reading-ogg-files-using-libogg.html
*/
if (System args size != 2, 
  write("requires name of ogg file as argument\n")
  System exit(0)
)

Decoder := Object clone do(
	streams := Map clone
	readPage := method(file, state,
		page := Ogg OggPage clone
		while (page != nil and state pageout(page) != 1,
			buffer := file readBufferOfLength(4096)
			if (buffer != nil, state write(buffer), page = nil)
		)
		page
	)
)
			
file := File clone openForReading(System args at(1))
state := Ogg OggSyncState clone
pages := 0
page := Decoder readPage(file, state);
while(page != nil,
	serial := page serialno asString
	if (page bos, Decoder streams atPut(serial, Ogg OggStreamState clone do(packets := 0; setSerialNumber(page serialno))))
	stream := Decoder streams at(serial)
	stream pagein(page)

	packet := Ogg OggPacket clone
	r := stream packetout(packet)
	while (r == 1,
		stream packets := stream packets + 1
		packet = Ogg OggPacket clone
		r := stream packetout(packet)
	)
		
	pages = pages + 1
	page = Decoder readPage(file, state);
)

write("Pages: ", pages, "\n");

Decoder streams foreach(serial, stream,
	write("serial: ", serial, " packets: ", stream packets, "\n")
)
file close
