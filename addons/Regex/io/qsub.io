Regex do(
  docSlot("SubstitutionRegex", "A Regex which matches substitution patterns.")
  SubstitutionRegex := Regex clone setPattern("(\\\\+)(\\d+)")
  
  docSlot("qsub(formatString)",
    "Perform substitution using the provided format string.
    The format string replaces \\N with the Nth group."
  )
  qsub := method(format,
    parts := list
    SubstitutionRegex setString(format) eachInterval(i,
      extraSlashes := i at(1) slice(0, ((i at(1) size - 1) / 2) floor)
      if(extraSlashes size > 0, parts append(extraSlashes))
      
      # Append a string if it is a normal digit, a number if it is a capture reference
      parts append(
        if(i at(1) size % 2 == 0, i at(2), i at(2) asNumber)
      ),
      parts append(i) # Append the inter-match string
    )
    
    self substitute(match,
      parts map(part,
        if(part hasProto(Number), match at(part) asString, part)
      ) join
    )
  )
)
