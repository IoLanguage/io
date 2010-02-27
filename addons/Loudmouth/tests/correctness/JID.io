JIDTest := UnitTest clone do(
  setUp := method(
    self cow := JID with("milka@moo.com/Alpes")
    self cat := JID with("bobo@lolcats.com"))
  
  testUsername := method(
    assertEquals(cow username, "milka")
    assertEquals(cat username, "bobo"))
  
  testHost := method(
    assertEquals(cow host, "moo.com")
    assertEquals(cat host, "lolcats.com"))
    
  testResource := method(
    assertEquals(cow resource, "Alpes")
    assertEquals(cat resource, nil))
  
  testAsString := method(
    assertEquals(cow asString, "milka@moo.com/Alpes")
    assertEquals(cat asString, "bobo@lolcats.com"))
)
