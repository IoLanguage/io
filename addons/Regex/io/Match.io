Range // needed for existing Match code to work

Regex do(
  docSlot("Match",
    "Match instances hold the subject string, and the ranges of each capture.
    The ranges are used to extract the entire match and each capture."
  )
  
  Match := Object clone do(
    newSlot("subject", "", "The entire string being matched against")
    newSlot("captures", list(), "A list holding each capture")
    newSlot("ranges", list(), "A list holding the range for each capture")
    
    docSlot("at(index)",
      "Access the capture with the provided index.
      match at(0) is the entire match, equivalent to match string"
    )
    at := method(index, captures at(index))
    
    docSlot("string", "The matched string; same as at(0)")
    asString := string := method(captures at(0))
    
    docSlot("range", "The range of the entire match in the subject.")
    range := method(ranges first)
    docSlot("begin", "Alias for range begin")
    begin := method(if(range, range first, nil))
    docSlot("end", "Alias for range end")
    end := method(if(range, range last, nil))
    
    docSlot("asCode", "A poorly named inspect method")
    asCode := method("<Regex Match: " .. match .. ">")
    
    docSlot("cloneWithSubject(subjectString)",
      "Create a clone of the receiever, clearing the ranges and captures lists."
    )
    cloneWithSubject := method(subject,
      self clone setSubject(subject) setCaptures(list) setRanges(list)
    )
    docSlot("addCaptureRange(begin, end)",
      "Add a new capture to the list of captures."
    )
    addCaptureRange := method(begin, end,
      ranges append(begin to(end))
      captures append(subject slice(begin, end))
    )
  )
  
  docSlot("match",
    "Return the current match, or if there is none, finds the next match and returns it."
  )
  match := method(
    if(match := self currentMatch, match, self nextMatch)
  )
)
