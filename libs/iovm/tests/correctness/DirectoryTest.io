DirectoryTest := UnitTest clone do(
    setUp := method(
        # Creating some test directories and files ...
        # testDir
        # |-- emptySubDir
        # |-- testFile1
        # `-- testSubDir
        #     `-- testFile2
        # ...  kind of  unreliable, because Directory createIfAbsent() itself
        # will be tested in this UnitTest.
        self dirs := list("testDir/testSubDir", "testDir/emptySubDir") map(path,
            Directory with(path) createIfAbsent
        )

        self files := list(
            "testDir/testFile.x", "testDir/testFile.y", "testDir/testSubDir/testFile.z"
        ) map(path, File with(path) create)

        # So we don't write the same line 100x times :)
        self testDir := Directory with("testDir")
    )

    testWith := method(
        assertRaisesException(Directory with)
        assertEquals(".", Directory with(".") path)
    )

    testDirectories := method(
        # Test case, where the directory ...
        # a) is empty,
        assertEquals(list(), Directory with("testDir/emptySubDir") directories)
        # b) has some subdirs,
        assertEquals(
            dirs map(name),
            Directory with("testDir") directories map(name)
        )
        # c) has some files, but no subdirs.
        assertEquals(list(), Directory with("testDir/testSubDir") directories)
    )

    testWalk := method(
        # Since Directory walk() uses List map() for executing callbacks on
        # each File object, we don't need to test that behaviour once again.

        # Testing the case where the directory is empty, expecting no
        # errors to be raised.
        assertNil(try(Directory with("testDir/emptySubDir")))

        # Checking that all the subdirectories are walked.
        found := list()
        testDir walk(f, found append(f name))
        assertEquals(3, found size)
        assertEquals(self files map(name), found)
    )

    testRecursiveFilesOfTypes := method(
        # Testing the case, where ...
        # a) a single file name is given,
        files := testDir recursiveFilesOfTypes(list(".x"))
        assertEquals(1, files size)
        assertEquals(list("testFile.x"), files map(name))
        # b) multiple file names are given.
        files := testDir recursiveFilesOfTypes(list(".x", ".y", ".z"))
        assertEquals(3, files size)
        # File object comparison is not implemented, so we have to compare
        # the files by the name :/
        assertEquals(self files map(name), files map(name))
    )

    testFilesWithExtension := method(
        assertRaisesException(testDir filesWithExtension)

        # Testing the case, where the directory ...
        # a) is empty,
        assertEquals(
            list(), Directory with("testDir/emptySubDir") filesWithExtension(".x")
        )
        # b) has files with a given extension,
        assertEquals(
            list("testFile.x"), testDir filesWithExtension(".x") map(name)
        )
        # c) doesn't have files with a given extension.
        assertEquals(
            list(), testDir filesWithExtension(".xyz")
        )
    )

    testIsAccessible := method(
        assertTrue(testDir isAccessible)
        assertFalse(Directory with("iDontExist") isAccessible)
    )

    tearDown := method(
        # Note: probably it makes sense to add Directory removeRecursive method,
        # as en equivalent to `rm -r ...` UNIX command.
        self files map(remove)
        self dirs map(remove)
        testDir remove
    )
)

if(isLaunchScript, DirectoryTest run, DirectoryTest)