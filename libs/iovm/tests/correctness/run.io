#!/usr/bin/env io

if(System args size > 1,
    # Run specific tests.
    System args slice(1) foreach(name,
        try(
            if(name endsWithSeq(".io"),
                # Use Path for platform-independent file joining
                Lobby doFile(Path with(System launchPath, name))
            ,
                Lobby doString(name)
            )
        ) ?showStack
    )
    System exit(FileCollector run size)
,
    # Run all tests in the current directory.
    System exit(DirectoryCollector run size)
)
