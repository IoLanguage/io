System temporaryDirectory := method(
    list("TMP", "TEMP", "TMPDIR") foreach(v,
        if (path := System getEnvironmentVariable(v), return path)
    )
)

ReadLineTest := UnitTest clone do(

    testCannotLoadHistoryFromNonexistentFile := method(
        historyFilePath := Path with(System temporaryDirectory, "io-nonexistent-history")
        assertRaisesException( ReadLine loadHistory(historyFilePath) )
    )

    testSaveHistoryFile := method(
        historyFilePath := Path with(System temporaryDirectory, "io-new-history")

        ReadLine addHistory("history line")
        ReadLine saveHistory(historyFilePath)

        historyFile := File with(historyFilePath)

        assertTrue(historyFile exists)
        assertEquals("history line", historyFile readLines last)

        historyFile remove
    )

    testCanLoadHistoryFromExistingFile := method(
        historyFilePath := Path with(System temporaryDirectory, "io-existing-history")
        historyFile := File with(historyFilePath)
        historyFile open
        historyFile write("history line")
        historyFile close

        ReadLine loadHistory(historyFilePath)
        ReadLine addHistory("history line 2")
        ReadLine saveHistory(historyFilePath)

        historyFile := File with(historyFilePath)

        assertEquals(list("history line", "history line 2"), historyFile readLines)

        historyFile remove
    )
)
