
// Try running this script with some cli args. Example:
// ./io test/CommandLineArgs.io a b c d

"Command line arguments:\n" print
Lobby args foreach(k, v, writeln("'", v))
