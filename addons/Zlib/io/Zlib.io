ZlibDecoder do(
	inputBuffer ::= nil
	outputBuffer ::= nil

	init := method(
	   setInputBuffer(Sequence clone)
	   setOutputBuffer(Sequence clone)
	)
)

Sequence unzip := method(
	z := ZlibDecoder clone
	z inputBuffer = self
	//writeln("z inputBuffer size = ", z inputBuffer size)
	z beginProcessing
	z process
	z endProcessing
	//writeln("z outputBuffer size = ", z outputBuffer size)
	self copy(z outputBuffer)
	self
)

Zlib := Object clone do(
	//metadoc Zlib category Compression
/*metadoc Zlib description
The Zlib object can be used to compress and uncompress data using the 
<a href=http://en.wikipedia.org/wiki/Zlib>zlib</a> 
lossless data compression algorithm.
<p>
Example use;
<pre>
compressedData := Zlib compress(uncompressedData)
uncompressedData := Zlib uncompress(compressedData)
</pre>	
*/
	ZlibDecoder := ZlibDecoder
	
	compress := method(s,
		//doc Zlib compress(aSeq) Returns a compressed version of aSeq.
		z := ZlibEncoder clone
		z inputBuffer = s
		z beginProcessing
		z process
		z endProcessing
		z outputBuffer	
	)
	
	uncompress := method(s,
		//doc Zlib uncompress(aSeq) Returns a uncompressed version of aSeq.
		z := ZlibDecoder clone
		z inputBuffer = s
		z beginProcessing
		z process
		z endProcessing
		z outputBuffer	
	)
)
