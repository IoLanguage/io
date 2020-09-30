#!/usr/bin/env io

if(System args size > 1,
    # Run specific tests.
    System args slice(1) foreach(name,
        try(
            if(name endsWithSeq(".io"),
                # FIXME: This is platform dependent!
                Lobby doFile(System launchPath .. "/" ..  name)
            ,
                Lobby doString(name)
            )
        ) ?showStack
    )
    FileCollector run size # size returns non-0 code on failure
,
    # Run all tests in the current directory.
    DirectoryCollector run size # size returns non-0 code on failure
)
