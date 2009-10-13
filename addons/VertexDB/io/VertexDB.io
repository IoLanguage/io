VertexDB := Object clone

File thisSourceFile parentDirectory directoryNamed("protos") files foreach(protoFile,
	//writeln(protoFile path)
	doFile(protoFile path) appendProto(VertexDB)
)