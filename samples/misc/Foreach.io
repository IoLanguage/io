#!/usr/bin/env io

a := List clone append(1, 2, 3)


b := method(
  a foreach(i, v, if(v == 2, return v))
  write("shouldn't get here\n")
)

write("b := ", b, "\n")		// => 2
