
Tank := Object clone
Tank parent := OpenGL
Tank position := Point clone
Tank angle := 90
Tank speed := 2
Tank radius := 20
Tank protoName := "Tank"

Tank init := method(
  self position := self position clone
)

Tank draw := method(
  glPushMatrix
  b := 40
  twidth := 10
  tlength := b *1.5
  glTranslated(position x, position y, 0)
  glRotated(angle-90, 0, 0, 1)
  glColor4d(.1, .7, .1, 1)
  glColor4d(.5,.5,.5, 1)
  glRectd(-b*.7, -b, b*.7, b)
  
  glColor4d(0, .5, 0, 1)
  glColor4d(.4,.4,.4, 1)
  glRectd(- twidth/2, 0, twidth/2, tlength)
  glRectd(-20, -20, 20, 20)
  glPopMatrix
)

Tank key := method(k, 
  d := 2* 3.14159 / 360
  if (k == "w", speed += .1)
  if (k == "s", speed -= .1)
  if (k == "a", angle += 2)
  if (k == "d", angle -= 2)
)

Tank timestep := method(
  position x += (d * angle) cos * speed
  position y += (d * angle) sin * speed
)

Tank checkCollision := method(other,
  if (position distanceTo(other position) < radius + other radius, 
    speed *= -1.1;
  )
)
