#!/usr/bin/env io

if (System args size != 2, 
  write("requires name of ogg file as argument\n")
  System exit(0)
)

Decoder := Object clone do(
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
	pages = pages + 1
	page = Decoder readPage(file, state);
)
write("Pages: ", pages, "\n");
file close
