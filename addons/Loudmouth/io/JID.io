Regex

JID := Object clone do(
  username  ::= nil
  host      ::= nil
  resource  ::= nil
  asString  ::= nil

  with := method(jid,
    matches := jid allMatchesOfRegex("(.+)@([^/]+)/?(.*)?") ?first ?captures
    matches at(3) isEmpty ifTrue(matches atPut(3, nil))
    
    self clone\
      setAsString(jid)\
      setUsername(matches at(1))\
      setHost(matches at(2))\
      setResource(matches at(3)))
)

Sequence asJid := method(
  JID with(self))
