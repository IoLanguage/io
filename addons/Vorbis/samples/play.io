#!/usr/bin/env io
/* 
   Based on code at 
   http://www.bluishcoder.co.nz/2009/06/decoding-vorbis-files-with-libvorbis.html
*/
if (System args size != 2, 
  write("requires name of ogg file as argument\n")
  System exit(0)
)

Stream := Object clone do(
        streamType := "unknown"
        streamState := Ogg OggStreamState clone
	info := Ogg Vorbis VorbisInfo clone
	comment := Ogg Vorbis VorbisComment clone
	dsp := Ogg Vorbis VorbisDspState clone
	block := Ogg Vorbis VorbisBlock clone
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
	readHeaders := method(file, state,
		    headersDone := false;
		    page := readPage(file, state);
		    while(headersDone not and page != nil,
		            serial := page serialno asString
			    if (page bos, 
			       streams atPut(serial, Stream clone)
			       stream := streams at(serial)
			       stream streamState setSerialNumber(serial asNumber)
			    )
			    stream := streams at(serial)
			    stream streamState pagein(page)
			    
			    packet := Ogg OggPacket clone
			    r := stream streamState packetpeek(packet)
			    while(headersDone not and r != 0,
			            r = stream dsp headerin(stream info, stream comment, packet)
				    if (stream streamType == "vorbis" and r == -132, headersDone = true)
				    if (r == 0, stream streamType = "vorbis")
				    
				    if (headersDone not, stream streamState packetout(packet))
			            r := stream streamState packetpeek(packet)	    
			    )
		            page := readPage(file, state);		    		      
		    )
		    self
		    
	}
	readPacket := method(file, state, stream,
		   packet := Ogg OggPacket clone
		   r := stream streamState packetout(packet)
		   while(r != 1,
		   	   page := readPage(file, state)
			   if (page == nil, packet = nil; break)
			   stream := streams at(page serialno asString)
			   stream streamState pagein(page)
		           r := stream streamState packetout(packet)
		   )
		   packet
	)
)
			
file := File clone openForReading(System args at(1))
state := Ogg OggSyncState clone
Decoder readHeaders(file, state);
write("Headers read\n");
audio := nil
Decoder streams foreach(serial, stream,
	if (audio == nil and stream streamType == "vorbis",
	   write("initializing vorbis stream ", serial, 
                 " rate ", stream info rate,
		 " channels ", stream info channels,
		 "\n")
	   stream dsp setup(stream info)
	   stream block setup(stream dsp, stream info)
	   audio = stream
	)
)

if (audio != nil,
   AudioDevice open
   packet := Decoder readPacket(file, state, audio)
   lastWrite := nil
   while(packet != nil,
      r := audio block synthesis(packet)
      if (r == 0, audio dsp blockin(audio block))
      buffer := audio dsp pcmout
      AudioDevice write(buffer);
      packet := Decoder readPacket(file, state, audio)
   )
)

Decoder streams foreach(serial, stream,
	write("serial: ", serial, "\n")
)
file close
