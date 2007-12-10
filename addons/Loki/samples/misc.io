
Protoless := Object clone do( 
	__indirectAppendProto := Object getSlot("appendProto")
	regenProto := method( __indirectAppendProto(__indirectAppendProto protos at(0)) )
	init := method( 
		regenProto = self getSlot("regenProto"); 
		__indirectAppendProto = self getSlot("__indirectAppendProto") 
	)
)
/*
Map atPush := method(k, v, if( at(k) isNil, atPut(k, list(v)), at(k) push(v)))

Sequence isHex := method(
	foreach(k,v, (
		v between("A" at(0),"F" at(0))) or(
		v between("a" at(0),"f" at(0))) or(
		v between("0" at(0),"9" at(0))) ifFalse(return false)
	)
	true
)*/
