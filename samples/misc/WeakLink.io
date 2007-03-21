
monitor := Object clone do(
    collectedWeakLink := method(wlink, 
      write("collected weak link #", wlink identifier, "\n")
    )
)

test := method(
  a := Object clone

  Lobby w := WeakLink clone setLink(a) 
  w setDelegate(monitor) setIdentifier(w uniqueId)
)

test
Collector collect