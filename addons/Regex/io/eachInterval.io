Regex do(
  docSlot("eachInterval(intervalName, matchMessage, interMatchMessage)",
    "Iterate each interval in the string. The first message is performed for
    each match; the second for each inter-match interval."
  )
  eachInterval := method(
    name := call message argAt(0) name
    cursor := 0
    eachMatch(match,
      if(match begin > cursor,
        call sender setSlot(name, string slice(cursor, match begin))
        call sender doMessage(call message argAt(2))
      )
      call sender setSlot(name, match)
      call sender doMessage(call message argAt(1))
      cursor := match end
    )
    
    # Perform the inter-match interval for the tail end
    tail := string slice(cursor)
    if(tail size > 0,
      call sender setSlot(name, tail)
      call sender doMessage(call message argAt(2))
    )
    self
  )
)