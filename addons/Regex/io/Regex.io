Regex do(
    docSlot("with(string, pattern)",
    "Returns a clone of the Regex object with the string set to the first argument and the pattern set to the second.")

    with := method(string, pattern,
        self clone setString(string) setPattern(pattern)
    )
)
