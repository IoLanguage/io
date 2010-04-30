SGML

LoudmouthMessage := Object clone do(
//metadoc LoudmouthMessage category Networking
//metadoc LoudmouthMessage description LoudmouthMessage provides [[SGML]] interface with few convience methods for manipulation of XMPP messages.
  original  ::= nil
  node      ::= nil
  xmppType  ::= nil
  receivedAt ::= nil

  //doc LoudmouthMessage with(source) Creates new [[LoudmouthMessage]] based on <code>source</code>, which should be valid XML ([[Sequence]]).
  with := method(source,
    now := Date now
    source = source asMutable removeSeq("\n") removeSeq("\t") removeSeq("\r")
    # Ignores possible whitespace
    parsed := source asXML subitems select(item, item name != nil) first

    self clone\
      setReceivedAt(now)\
      setOriginal(source)\
      setNode(parsed))

  //doc LoudmouthMessage to Returns [[JID]] of receiver.
  to := method(
    self node attributes at("to") ?asJid)

  //doc LoudmouthMessage setTo(jid) Sets message receiver to defined JID. JID can be both an [[Sequence]] or a [[JID]] object. Returns <code>self</code>.
  setTo := method(jid,
    self node attributes atPut("to", jid asString)
    self)

  //doc LoudmouthMessage from Returns sender's JID.
  from := method(
    self node attributes at("from") ?asJid)

  //doc LoudmouthMessage setFrom(jid) Sets sender's JID. Returns <code>self</code>.
  setFrom := method(jid,
    self node attributes at("from", jid asString)
    self)

  //doc LoudmouthMessage sendVia(loudmouthConnection) Converts message to sequence and sends it via the provided [[Loudmouth]] object. It updates <code>from</code> property and returns <code>true</code> if message is sent.
  sendVia := method(account,
    self setFrom(account jid)
    account sendRaw(self asString))

  //doc LoudmouthMessage xmppType Returns value of <code>type</code> attribute of message node.
  xmppType := method(
    self node attributes at("type"))

  //doc LoudmouthMessage setXmppType(type) Sets value of <code>type</code> attribute.
  setXmppType := method(_xmppType,
    self node attributes atPut("type", _xmppType)
    self)

  //doc LoudmouthMessage body Returns value message's body node. If both XHTML and plain versions are available, XHTML version will be returned. 
  body := method(
    if(self ?xhtmlBody isNil, self plainBody, self xhtmlBody))

  //doc LoudmouthMessage plainBody Returns value of message's body node.
  plainBody := method(
    self node subitems select(item,
      (item name == "body") and((item attributes at("xmlns") ?containsSeq("xhtml")) not)) first ?allText)

  //doc LoudmouthMessage plainBody Returns value of message's body node with XHTML markup.
  xhtmlBody := doString(getSlot("plainBody") code asMutable replaceSeq("not", ""))

  //doc LoudmouthMessage asString Converts XML to Sequence.
  asString := method(
    self node asString)
)

XmppChatMessage := LoudmouthMessage clone do(
//metadoc XmppChatMessage category Networking
//metadoc XmppChatMessage description A convience object for creating XMPP messages with chat type.
//doc XmppChatMessage create Creates a new [[LoudmouthMessage]] of chat <code>type</code> and random <code>id</code> value.
  create := method(
    super(with("""<message from="" to="" id="lm.io.#{self uniqueHexId}" type="chat"></message>""" interpolate)))
)
