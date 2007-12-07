

Foo := Object clone do(

  ray := method(
    4 + 6
  )
 
  bar := method(
    y := 3
    x := 5 + ray * y
    writeln("x: ", x)
  )
)
