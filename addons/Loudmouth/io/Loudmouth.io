Loudmouth do(
//metadoc Loudmouth category Networking
/*metadoc Loudmouth description
<a href="http://groups.google.com/group/loudmouth-dev/">Lodumouth</a> is an async XMPP library written in C.

Example usage:
<code><pre>
acc := Loudmouth with("user@server.com", "super password") do(
  handleConnect = method(
    "Connected!" println)

  handleMessage = method(msg
    "#{msg from} > #{msg plainBody}" println
    body :=  msg plainBody

    if(body indexOf("#") == 0,
      body = doString(body) asString)

    # This way you can manipulate
    # XML nodes with SGML addon
    XmppChatMessage create\
      setPlainBody(body)\
      setTo(msg from)\
      sendVia(self)

    # or simply send the message (must be a Sequence)
    # (this is obviously faster)
    #self send(msg from, body))
)

acc connect
# Any Io code after this line won't be executed
# (unless called as Loudmouth callback or run in separate thread)
Loudmouth startMainLoop

</pre></code>*/
  /*doc Loudmouth types
  <p>Object containing status codes and message types.</p>
  <pre><code>
  Loudmouth with("username@server.com", "password") do(
    handleConnect = method(
      self setPresence(Loudmouth types AVAILABLE), "Drinking lemonade...")
  )
  </code></pre>*/
  types := Object clone do(
    forward := method(
      msgName := call message name
      self setSlot(msgName, msgName asLowercase))
  )

  jid       ::= nil
  username  ::= nil
  password  ::= nil
  resource  ::= "loudmouth-io"
  host      ::= nil
  port      ::= 5222
  useSsl    ::= false
  presence  := "available"
  statusMessage := nil
  _msgsBuffer::= nil

  init := method(
    super(init)
    self _msgsBuffer = list())

  //doc Loudmouth with(jid, password) Creates a new Lodumouth clone with server details provided in <code>jid</code>.
  with := method(jid, password,
    self clone\
      setJid(jid asString)\
      setUsername(jid username)\
      setPassword(password)\
      setHost(jid host)\
      setResource(jid resource))

  //doc Loudmouth handleConnect Slot called once connection is established.
  handleConnect         := method()
  //doc Loudmouth handleConnectFailure Slot called when server is not reachable.
  handleConnectFailure  := method()
  //doc Lodumouth handleAuthenticated Slot called upon successful authentication.
  handleAuthenticated   := method()
  //doc Loudmouth handleAuthenticationFailure Slot called if username/password combinaton is wrong.
  handleAuthenticationFailure := method()
  //doc Loudmouth handleDisconnect Slot called upon closing the connection with the server.
  handleDisconnect      := method()
  //doc Loudmouth handleSslFailure Slot called if SSL-related problems arrive. Method should return <code>true</code> if it wishes to make a connection without SSL, <code>false</code> otherwise.
  handleSslFailure      := method(true)
  //doc Loudmouth handleMessage(xmppMessage) Slot called when a message arrives via XMPP stream. <code>xmppMessage</code> is a <code>LoudmouthMessage</code> object.
  # Overwrite this method in your clone
  handleMessage         := method()

  parseMessage := method(
    while(self _msgsBuffer isEmpty not,
      e := try(self handleMessage(LoudmouthMessage with(self _msgsBuffer removeFirst)))
      e catch(e println)))

  //doc Loudmouth setPresence(presence[, statusMessage]) Sets availability/presence and status message.
  setPresence := method(_presence, msg,
    msg ifNil(msg = self statusMessage ifNil(""))

    self presence = _presence
    self statusMessage = msg
    self updatePresenceAndStatusMsg(_presence, msg)
    self)

  //doc Loudmouth setStatusMessage(statusMessage) Sets status message.
  setStatusMessage := method(msg
    self setPresence(self presence, msg))
)
