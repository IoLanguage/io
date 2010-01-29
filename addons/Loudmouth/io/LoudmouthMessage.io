SGML

LoudmouthMessage := Object clone do(
  original  ::= nil
  node      ::= nil
  xmppType  ::= nil
  receivedAt ::= nil

  with := method(source,
    now := Date now
    source = source asMutable removeSeq("\n") removeSeq("\t") removeSeq("\r")
    # Ignores possible whitespace
    parsed := source asXML subitems select(item, item name != nil) first

    self clone\
      setReceivedAt(now)\
      setOriginal(source)\
      setNode(parsed))

  to := method(
    self node attributes at("to"))

  setTo := method(jid,
    self node attributes atPut("to", jid asString)
    self)

  from := method(
    self node attributes at("from"))

  setFrom := method(jid,
    self node attributes at("from", jid asString)
    self)

  send := method(account,
    self setFrom(account jid)
    account sendRaw(self asString))

  xmppType := method(
    self node attributes at("type"))

  setXmppType := method(_xmppType,
    self node attributes atPut("type", _xmppType)
    self)

  body := method(
    if(self ?xhtmlBody isNil, self plainBody, self xhtmlBody))

  plainBody := method(
    self node subitems select(item,
      (item name == "body") and((item attributes at("xmlns") ?containsSeq("xhtml")) not)) first ?allText)

  xhtmlBody := doString(getSlot("plainBody") code asMutable replaceSeq("not", ""))

  asString := method(
    self node asString)
)

XmppChatMessage := LoudmouthMessage clone do(
  with := method(
    super(with("""<message from="" to="" id="lm.io.#{self uniqueHexId}" type="chat"></message>""" interpolate)))
)
