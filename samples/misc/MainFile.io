#!/usr/bin/env io

if(isLaunchScript,
    # Run from the command line: io MainFile.io
    writeln("Wasn't included from another file")
    writeln

    writeln("System launchPath: ", System launchPath)
    writeln("System launchScript: ", System launchScript)
    writeln("System args: ", System args)
,
    # Loaded using doFile("MainFile.io")
    writeln("Was included from another file")
)
