RegexTest := UnitTest clone do(
  assertEqual := getSlot("assertEquals")

  testWithoutCaptures := method(
    r := Regex clone setString("Hello World") setPattern("\\w+")
    first := r nextMatch
    assertEqual("Hello", first string)
    assertEqual(list("Hello"), first captures)
    assertEqual(first, r currentMatch)
    assertTrue(first == r currentMatch)
    
    last := r nextMatch
    assertEqual("World", last string)
    assertTrue(last == r currentMatch)
    
    # Make sure they're not the same match
    assertTrue(first != last)
    assertEqual("Hello", first string)
  )
  
  testWithCaptures := method(
    r := Regex clone setString("37signals 43things 69pictures") setPattern("(\\d+)(\\w+)")
    
    s := r nextMatch
    assertEqual("37signals", s at(0))
    assertEqual("37", s at(1))
    assertEqual("signals", s at(2))
    
    t := r nextMatch
    assertEqual("43things", t at(0))
    assertEqual("43", t at(1))
    assertEqual("things", t at(2))

    p := r nextMatch
    assertEqual("69pictures", p at(0))
    assertEqual("69", p at(1))
    assertEqual("pictures", p at(2))
  )
  
  testHasMatch := method(
    r := Regex clone setString("Hey you!") setPattern("[A-Za-z]+")
    assertTrue(r hasMatch)
    r setString("12 34")
    assertFalse(r hasMatch)
  )
  
  testAllMatches := method(
    r := Regex clone setString("Hey you!") setPattern("\\w+")
    matches := r allMatches
    assertEqual(2, matches size)
    assertEqual("Hey", matches first string)
    assertEqual("you", matches last string)
  )
  
  testEachMatch := method(
    r := Regex clone setString("Hey you!") setPattern("\\w+")
    record := list
    r eachMatch(m, record append(m))
    assertEqual(2, record size)
    assertEqual("Hey", record first string)
    assertEqual("you", record last string)
  )
  
  testEachInterval := method(
    r := Regex clone setString("Hey you!") setPattern("\\w+")
    record := list
    r eachInterval(m, record append(m), record append(m))
    assertEqual(4, record size)
    assertEqual("Hey", record at(0) string)
    assertEqual(" ", record at(1))
    assertEqual("you", record at(2) string)
    assertEqual("!", record at(3))
  )
  
  testSubstitute := method(
    r := Regex clone setString("Hello32 World10") setPattern("\\d+")
    assertEqual("Hello42 World20", r substitute(m, m string asNumber + 10))
  )
  
  testEscape := method(
    assertEqual("Hello", Regex escape("Hello"))
    assertEqual("\\\\d", Regex escape("\\d"))
    assertEqual("\\(\\.\\+\\)", Regex escape("(.+)"))
  )
  
  testNumberOfCaptures := method(
    assertEqual(0, Regex clone setPattern(".*") numberOfCaptures)
    assertEqual(0, Regex clone setPattern("\\d+.*\\w+") numberOfCaptures)
    assertEqual(1, Regex clone setPattern("(\\d+).*\\w+") numberOfCaptures)
    assertEqual(2, Regex clone setPattern("(\\d+).*(\\w+)") numberOfCaptures)
    assertEqual(3, Regex clone setPattern("(\\d+)(.*)(\\w+)") numberOfCaptures)
    assertEqual(4, Regex clone setPattern("(\\d+)(.*)()(\\w+)") numberOfCaptures)
  )
  
  testQuickSub := method(
    r := Regex clone setString("37signals 43things 69pictures") setPattern("(\\d+)(\\w+)")
    assertEqual("=signals-37= =things-43= =pictures-69=", r qsub("=\\2-\\1="))
    assertEqual("\\37 \\43 \\69", r qsub("\\\\\\1"))
    assertEqual("1 1 1", r qsub("\\\\1"))
  )
)

RegexMatchTest := UnitTest clone do(
  setUp := method(
    super(setUp)
    self regex := Regex clone setString("37signals 43things 69pictures") setPattern("(\\d+)(\\w+)")
    self match := regex match
  )
  
  testRange := method(
    assertEquals(0, match begin)
    assertEquals(9, match end)
    assertEquals(0, match range begin)
    assertEquals(9, match range end)
  )
  
  testCaptures := method(
    assertEquals("37", match at(1))
    assertEquals("signals", match at(2))
    assertEquals("37signals", match at(0))
    assertEquals("37signals", match string)
  )
)

RegexTest run
RegexMatchTest run
"Tests finished" println