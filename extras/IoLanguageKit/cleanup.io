

Directory directoryNamed("source") directoryNamed("lib") directoryNamed("io") directoryNamed("addons") directories foreach(d,
	writeln("cleaning ", d name)
	
	try(d directoryNamed("source") remove)
	d fileNamed("protos") remove
	try(d fileNamed("depends") remove)
	try(d directoryNamed("tests") remove)
	try(d directoryNamed("samples") remove)
	d fileNamed("CMakeLists.txt") remove
	
	try(d directoryNamed("_build") directoryNamed("binaries") remove)
	try(d directoryNamed("_build") directoryNamed("lib") remove)
	try(d directoryNamed("_build") directoryNamed("objs") remove)
)
