Regex do(
  docSlot("allMatches",
    "Returns a List of strings containing all matches of receiver's pattern within its string."
  )
  allMatches := method(
    matches := list
    resetSearch
    while(match := self nextMatch, matches append(match))
    matches
  )
)