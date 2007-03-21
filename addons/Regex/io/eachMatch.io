Regex do(
  docSlot("eachMatch(aName, doSomethingWithAName)",
    "Perform the given message with aName set to each match."
  )
  eachMatch := method(
    name := call message argAt(0) name
    iterator := call message argAt(1)
    resetSearch
    while(match := nextMatch,
      call sender setSlot(name, match)
      call sender doMessage(iterator)
    )
  )
)