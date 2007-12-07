// AudioPad inspired Io Audio demo
// Steve Dekorte, 2003

Thing := Object clone
Thing appendProto(GLUT)
Thing pos := Point clone
Thing size := 40
Thing color := Color clone set(.5, .5, .5, .1) 
Thing isSelected := nil
Thing select := method(self isSelected := 1)
Thing unselect := method(self isSelected := nil)
Thing name := nil
Thing setName := method(n, self name := n; self)
Thing font := method(self font := sansFont24)
Thing q := method(self q := gluNewQuadric)
Thing init := method(
  self color := color clone
  self pos := Point clone
)

Thing display := method(
  glPushMatrix
  pos glTranslate
  self draw
  glPopMatrix
)

Thing draw := method(
  color glColor
  jitterDraw(gluDisk(q, 0, size, 64, 1))
  if (isSelected, jitterDraw(gluDisk(q, size * .8, size, 64, 1)))
  self drawName
)

Thing drawName := method(
  if (name == nil, return) 
  glColor4d(0, 0, 1, .8)
  glPushMatrix
  glTranslated( - font widthOfString(name)/2, 5- font pixelHeight/2, 0)
  font drawString(name)
  glPopMatrix
)

Thing distanceToPoint := method(p, pos distanceTo(p))
Thing touchesPoint := method(p, pos distanceTo(p) < size)

// -----------------------------------------------------------------------------

Speaker := Thing clone
Speaker color set(0, 0, 1, .2)
Speaker draw := method(
  Thing getSlot("draw") performOn(self)
)

// -----------------------------------------------------------------------------

Loop := Thing clone
Loop color set(.5, .5, .5, 0.15)
Loop angle := 0
Loop font := method(Loop font := sansFont12)

Loop init := method(
  Thing getSlot("init") performOn(self)
  self angle := 0
  self currentPosition := 0
  self lastPosition := 0
)

Loop open := method(name,
  self name := name lastPathComponent splitNoEmpties(".") first
  write("loading ", self name, "...")
  File standardOutput flush
  self sound := Sound clone open(name) loopingOn
  write("\n")
  return self
)

Loop start := method(
  last := AudioMixer sounds last
  if (last) then(
    AudioMixer addSoundOnSampleOfSound(sound, 0, last)
  ) else (
    AudioMixer addSound(sound)
  )
)

Loop sizeToIntegerMultipleOfLoop := method(otherLoop,
  otherSound := otherLoop sound
  s := sound sampleCount
  a := s
  b := s
  otherCount := otherSound sampleCount
  while ( a % otherCount != 0 and otherCount % a != 0,  a = a +1 )
  while ( b % otherCount != 0 and otherCount % b != 0,  b = b +1 )
  r := if((a-s) abs < (b-s) abs, a, b)
  if (r == 0, r := a)
  if (r != s,
    write("resampling ", name, " from ", s, " to ", r, "...")
    //write("ratios := ", otherCount/r, " ", r/otherCount,"\n")
    File standardOutput flush
    sound resampleToSampleCount(r) 
    write("\n")
  )
)


Loop drawName := method(
  if (name == nil, return) 
  glPushMatrix
  v := sound channelVolume(0) + sound channelVolume(1)
  if (v > 0, glColor4d(1, 1, 1, 1), glColor4d(1, 1, 1, .4))
  glTranslated( - font widthOfString(name)/2, - font pixelHeight / 2, 0)
  font drawString(name)
  glPopMatrix
)


Loop timer := method(
  sp := sound position
  if (lastPosition != sp) then(
    currentPosition = sound position 
    lastPosition = currentPosition
  ) else (
    currentPosition = currentPosition + (44100 / Mix framesPerSecond)
  )
)

Loop draw := method(
  Thing getSlot("draw") performOn(self)
  v := sound channelVolume(0) + sound channelVolume(1)
  if (v == 0, return) 
  v = v * .5
  d := size * 2 + 80 * v
  //cAlpha := aveValue sqtr * (v * v) + 0.3;
  alpha := v * .45 + .4
 
  glColor4d(1, 1, 0, alpha * 0.2)
  angle := 360 * currentPosition / sound sampleCount
  glRotated(-angle, 0, 0, 1)
  jitterDraw(
    gluPartialDisk(q, d, d + 40*2*v, 64, 1, 0, 180) 
  )
)

Loop updateLeftSpeaker := method(speaker,
  v := 1 - speaker distanceToPoint(pos) / 500
  v = v clip(0,1)
  sound setChannelVolume(0, v)
)

Loop updateRightSpeaker := method(speaker,
  v := 1 - speaker distanceToPoint(pos) / 500
  v = v clip(0,1)
  sound setChannelVolume(1, v)
)


