#!/usr/bin/env io

if (System args size != 2, 
  write("requires name of ogg file as argument\n")
  System exit(0)
)

file := File clone openForReading(System args at(1))
state := Ogg OggSyncState clone
page := Ogg OggPage clone
pages := 0
r := 0
buffer := file readBufferOfLength(4096);

while(true,
	if (buffer != nil, state write(buffer))
        r := state pageout(page);
	
	if (r == 1, pages = pages + 1; page granulepos println)
	buffer = file readBufferOfLength(4096)
	if (buffer == nil and r != 1, break)
)
write("Pages: ", pages, "\n");
file close
