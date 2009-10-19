VertexDB := Object clone

File thisSourceFile parentDirectory directoryNamed("protos") files foreach(protoFile,
	doFile(protoFile path) appendProto(VertexDB)
)

VertexDB Settings setHost("localhost") setPort(8080)
