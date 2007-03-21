Regex do(
  docSlot("escape(aString)",
    "Escape the provided string such that
      Regex clone setString(string) setPattern(Regex escape(string))
    will match the entire string."
  )
  RegexSpecials := Regex clone setPattern("[-*+?.()\\[\\]\\{\\}\\\\]")
  escape := method(string,
    RegexSpecials setString(string) substitute(match, "\\" .. match)
  )
)