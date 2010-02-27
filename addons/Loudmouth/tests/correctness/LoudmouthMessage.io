LoudmouthMessageTest := UniTest clone do(
  setUp := method(
    self msg := XmppChatMessage create do(
      setFrom("milka@moo")
      setTo("beaver@river")
#      setPlainBody("Hai!")
#      setXhtmlBody("<b>Hai!</b>")
    ))

  testNode := method(
    assertNotNil(self msg node)
    assertEquals(self msg node length, 1))
)
