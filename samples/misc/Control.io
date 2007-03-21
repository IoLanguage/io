// example of implementing control structures with blocks
// Note: if a block arg name begins with "_" 
// the arg message is passed instead of being evaluated

myif := method(a, _b, _c,
  if (a, sender doMessage(_b), sender doMessage(_c))
)


myif(1, write("true\n"), write("false\n"))


