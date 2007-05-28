/* The Computer Language Shootout
   http://shootout.alioth.debian.org
   -- Loki demo -- */

Loki Implementation clone linkToAs( Sequence, "nsieveBits" ) DBG with(

	bytes := _self data.ptr castAs(ByteArray) bytes castAs(WordPtr)
	n := int <- valueArgAt(0) data.d

//	bytes := int <- bytes
//	bytes := (bytes + 256) - (bytes&255) castAs(WordPtr)

	appendProto(x86 symbols)

//	one := int <- 1
	
//	proxyBlock asm( movd mm1, one )
//	proxyBlock asm( pxor mm0, mm0 )

	one := int64 <- 1
	count := int64 <- 0
//	cnt := int <- 0
	Loop as(i := int <- 2) while(i<n) by(i++) with(
		(((bytes[i>>5]) & ((int(1))<<(i&31))) == 0) ifTrue(
		//	Loop for(k := i+i; k<n; k += i) with( (bytes[k>>5] |= ((int(1))<<(k&31))) ! )
		
			d := (n+i-1)/i
		
			k := i+i
//			Loop for(k := i+i; k<n; k += i) with(
		//	Loop for(kk := int <- d-2; kk>0; kk --) with(
			Loop as(kk := int <- d-2) toZero with(
			//	k := kk * i
			
		//	idx := ((bytes + (k>>6)) <<3)
			idx := bytes + ((k>>6) <<3)
			s := int64 <- (k&63)
			nv := idx int64Get | (one<<s)
			idx int64Put( nv )
			
			k += i
			
		//	(bytes[k>>5] |= ((int(1))<<(k&31))) ! 
			)
		
			count += one
		//	cnt++
//			proxyBlock asm ( inc cnt )
//			proxyBlock asm ( paddd mm0,mm1 )
			)
		)

//	proxyBlock asm ( int3 )
//	proxyBlock asm ( paddq mm0,mm1 )
//	proxyBlock asm ( movd cnt, mm0 )

	cnt := int <- count
//	cnt := int <- (bytes&1023)
	proxyBlock asm ( emms )

	
	_self state numberWithDouble(cnt asFloat64)
	
)

nsieve := method(n,
	primes := Sequence clone setSize(n+1+1024) zero
	cnt := primes nsieveBits(n)
	
	( "Primes up to" .. n asString alignRight(9, " ") .. cnt asString alignRight(9, " ") ) println
)

//clk := 
//chrono(
t := Chrono clone
t measure(
	nsieve( (2**(n))*10000 )
	nsieve( (2**(n-1))*10000 )
	nsieve( (2**(n-2))*10000 )
);

t








