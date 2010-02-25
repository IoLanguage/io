
method(
	m := message(
		inputBuffer ::= nil
		outputBuffer ::= nil

		init := method(
		   setInputBuffer(Sequence clone)
		   setOutputBuffer(Sequence clone)
		)
	)

	LZODecoder doMessage(m)
	LZOEncoder doMessage(m)
) call

Sequence do(
	zCompress := method(
		z := LZOEncoder clone
		z inputBuffer = self
		z beginProcessing process endProcessing
		self copy(z outputBuffer)
		self
	)

	zUncompress := method(
		z := LZODecoder clone
		z inputBuffer = self
		z beginProcessing process endProcessing
		self copy(z outputBuffer)
		self
	)
)

LZO := Object clone do(
	//metadoc LZO category Compression
/*metadoc LZO description
The LZO object can be used to compress and uncompress data using the 
<a href=http://en.wikipedia.org/wiki/Lempel-Ziv-Oberhumer> Lempel-Ziv-Oberhumer (LZO)</a> 
lossless data compression algorithm.
<p>
Example use;
<pre>
compressedData := LZO compress(uncompressedData)
uncompressedData := LZO uncompress(compressedData)
</pre>	
*/

	LZOEncoder := LZOEncoder
	LZODecoder := LZODecoder
	
	compress := method(s,
		//doc LZO compress(aSeq) Returns a compressed version of aSeq.
	
		z := LZOEncoder clone
		z inputBuffer = s
		z beginProcessing process endProcessing
		z outputBuffer 		
	)
	
	uncompress := method(s,
		//doc LZO uncompress(aSeq) Returns a uncompressed version of aSeq.
		z := LZODecoder clone
		z inputBuffer = s
		z beginProcessing process endProcessing
		z outputBuffer 		
	)
)
