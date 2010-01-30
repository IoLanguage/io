SGML

LoudmouthMessage := Object clone do(
//metadoc LoudmouthMessage category Networking
/*metadoc LoudmouthMessage description
<p>LoudmouthMessage provides SGML interface and convience methods for manipulation of XMPP messages.</p>*/
  original  ::= nil
  node      ::= nil
  xmppType  ::= nil
  receivedAt ::= nil

  //doc LoudmouthMessage with(source) Creates new LoudmouthMessage based on <code>source</code>, which should be valid XML (<code>Sequence</code>).
  with := method(source,
    now := Date now
    source = source asMutable removeSeq("\n") removeSeq("\t") removeSeq("\r")
    # Ignores possible whitespace
    parsed := source asXML subitems select(item, item name != nil) first

    self clone\
      setReceivedAt(now)\
      setOriginal(source)\
      setNode(parsed))

  //doc LoudmouthMessage to Returns JID of receiver.
  to := method(
    self node attributes at("to") ?asJid)

  //doc LoudmouthMessage setTo(jid) Sets message receiver to defined JID. JID can be both an <code>Sequence</code> or <code>JID</code> instance. Returns <code>self</code>.
  setTo := method(jid,
    self node attributes atPut("to", jid asString)
    self)

  //doc Loudmouth from Returns sender's JID.
  from := method(
    self node attributes at("from") ?asJid)

  //doc Loudmouth setFrom(jid) Sets sender's JID. Returns <code>self</code>.
  setFrom := method(jid,
    self node attributes at("from", jid asString)
    self)

  //doc Loudmouth sendVia(loudmouthConnection) Converts message to sequence and sends it via provided Loudmouth connection (<code>Loudmouth</code> object). It updates <code>from</code> property and returns <code>true</code> if message is sent.
  sendVia := method(account,
    self setFrom(account jid)
    account sendRaw(self asString))

  //doc Loudmouth xmppType Returns value of <code>type</code> attribute of message node.
  xmppType := method(
    self node attributes at("type"))

  //doc Loudmouth setXmppType(type) Sets value of <code>type</code> attribute.
  setXmppType := method(_xmppType,
    self node attributes atPut("type", _xmppType)
    self)

  //doc Loudmouth body Returns value message's body node. If both XHTML and plain versions are available, XHTML version will be returned. 
  body := method(
    if(self ?xhtmlBody isNil, self plainBody, self xhtmlBody))

  //doc Loudmouth plainBody Returns value of message's body node.
  plainBody := method(
    self node subitems select(item,
      (item name == "body") and((item attributes at("xmlns") ?containsSeq("xhtml")) not)) first ?allText)

  //doc Loudmouth plainBody Returns value of message's body node with XHTML markup.
  xhtmlBody := doString(getSlot("plainBody") code asMutable replaceSeq("not", ""))

  //doc Loudmouth asString Converts XML to Sequence.
  asString := method(
    self node asString)
)

XmppChatMessage := LoudmouthMessage clone do(
//metadoc XmppChatMessage category Networking
  //doc XmppChatMessage create() Creates a new <code>LoudmouthMessage</code> with chat <code>type</code> and random id value.
  create := method(
    super(with("""<message from="" to="" id="lm.io.#{self uniqueHexId}" type="chat"></message>""" interpolate)))
)
