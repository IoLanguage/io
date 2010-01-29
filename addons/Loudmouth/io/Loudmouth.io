Loudmouth do(
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

  with := method(jid, password,
    self clone\
      setJid(jid asString)\
      setUsername(jid username)\
      setPassword(password)\
      setHost(jid host)\
      setResource(jid resource))

  handleConnect         := method()
  handleConnectFailure  := method()
  handleAuthenticated   := method()
  handleAuthenticationFailure := method()
  handleDisconnect      := method()
  handleSslFailure      := method(true)
  # Overwrite this method in your clone
  handleMessage         := method()

  parseMessage := method(
    e := try(self handleMessage(LoudmouthMessage with(self _msgsBuffer removeFirst)))
    e catch(e println)
    e)

  setPresence := method(_presence, msg,
    msg ifNil(msg = "")
    self presence = _presence
    self statusMessage = msg
    self updatePresenceAndStatusMsg(_presence, msg)
    self)

  setStatusMessage := method(msg
    self setPresence(self presence, msg))
)