#include "IoLoudmouth.h"
#define LMCONN(self) ((LmConnection *) IoObject_dataPointer(self))

/*** Helper functions ***/
LmMessageType str2msg_type(char *str) {
  if(!strcmp(str, "message"))      return LM_MESSAGE_TYPE_MESSAGE;
  if(!strcmp(str, "presence"))     return LM_MESSAGE_TYPE_PRESENCE;
  if(!strcmp(str, "iq"))           return LM_MESSAGE_TYPE_IQ;
  if(!strcmp(str, "stream"))       return LM_MESSAGE_TYPE_STREAM;
  if(!strcmp(str, "stream_error")) return LM_MESSAGE_TYPE_STREAM_ERROR;

  // Other LM_MESSAGE_TYPE_* stuff is mareked as internal and therefore
  // we won't need it.

  return LM_MESSAGE_TYPE_UNKNOWN;
}

LmMessageSubType str2msg_subtype(char *str) {
  if(!strcmp(str, "normal"))        return LM_MESSAGE_SUB_TYPE_NORMAL;
  if(!strcmp(str, "chat"))          return LM_MESSAGE_SUB_TYPE_CHAT;
  if(!strcmp(str, "group_chat"))    return LM_MESSAGE_SUB_TYPE_GROUPCHAT;
  if(!strcmp(str, "available"))     return LM_MESSAGE_SUB_TYPE_AVAILABLE;
  if(!strcmp(str, "headline"))      return LM_MESSAGE_SUB_TYPE_HEADLINE;
  if(!strcmp(str, "unavaiable"))    return LM_MESSAGE_SUB_TYPE_UNAVAILABLE;
  if(!strcmp(str, "probe"))         return LM_MESSAGE_SUB_TYPE_PROBE;
  if(!strcmp(str, "subscribe"))     return LM_MESSAGE_SUB_TYPE_SUBSCRIBE;
  if(!strcmp(str, "unsubscribe"))   return LM_MESSAGE_SUB_TYPE_UNSUBSCRIBE;
  if(!strcmp(str, "subscribed"))    return LM_MESSAGE_SUB_TYPE_SUBSCRIBED;
  if(!strcmp(str, "unsubscribed"))  return LM_MESSAGE_SUB_TYPE_UNSUBSCRIBED;
  if(!strcmp(str, "iq_get"))        return LM_MESSAGE_SUB_TYPE_GET;
  if(!strcmp(str, "iq_set"))        return LM_MESSAGE_SUB_TYPE_SET;
  if(!strcmp(str, "iq_result"))     return LM_MESSAGE_SUB_TYPE_RESULT;
  if(!strcmp(str, "error"))         return LM_MESSAGE_SUB_TYPE_ERROR;

  return LM_MESSAGE_SUB_TYPE_ERROR;
}


/***  Loudmouth callbacks ***/
void onXmppConnect(LmConnection *connection, int success, void* data) {
  IoObject *self = data;
  IoMessage *m;
  if(success == 1) {
    m = IoMessage_newWithName_label_(IOSTATE, IOSYMBOL("handleConnect"), IOSYMBOL("Loudmouth"));
    IoMessage_locals_performOn_(m, self, self);

    lm_connection_authenticate(
      connection,
      CSTRING(IoObject_getSlot_(self, IOSYMBOL("username"))),
      CSTRING(IoObject_getSlot_(self, IOSYMBOL("password"))),
      CSTRING(IoObject_getSlot_(self, IOSYMBOL("resource"))),
      onXmppAuth, data, NULL, NULL
    );
  } else {
    m = IoMessage_newWithName_label_(IOSTATE, IOSYMBOL("handleConnectFailure"), IOSYMBOL("Loudmouth"));
    IoMessage_locals_performOn_(m, self, self);
  }
}

void onXmppAuth(LmConnection *connection, int success, void* data) {
  IoObject *self = data;
  IoMessage *m;

  if(success == 1) {
    m = IoMessage_newWithName_label_(IOSTATE, IOSYMBOL("handleAuthenticated"), IOSYMBOL("Loudmouth"));
  } else {
    m = IoMessage_newWithName_label_(IOSTATE, IOSYMBOL("handleAuthenticationFailure"), IOSYMBOL("Loudmouth"));
  }

  IoMessage_locals_performOn_(m, self, self);
}

void onXmppDisconnect(LmConnection *connection, LmDisconnectReason reason, void* data) {
  IoObject *self = data;
  IoMessage *m = IoMessage_newWithName_label_(IOSTATE, IOSYMBOL("handleDisconnect"), IOSYMBOL("Loudmouth"));
  IoMessage_locals_performOn_(m, self, self);
}

LmHandlerResult onXmppMessage
  (LmMessageHandler *handler, LmConnection *connection, LmMessage *m, void* data)
{
  IoObject *self = data;

  IoList_rawAppend_(
    (IoList *)IoObject_getSlot_(self, IOSYMBOL("_msgsBuffer")),
    IOSYMBOL(lm_message_node_to_string(m->node))
  );

  IoMessage *io_m = IoMessage_newWithName_label_(IOSTATE, IOSYMBOL("parseMessage"), IOSYMBOL("Loudmouth"));
  IoMessage_locals_performOn_(io_m, self, self);

  return LM_HANDLER_RESULT_REMOVE_MESSAGE;
}

LmSSLResponse onSslError(LmSSL *ssl, LmSSLStatus status, void* data) {
  IoObject *self = data;

  IoMessage *m = IoMessage_newWithName_label_(IOSTATE, IOSYMBOL("handleSslError"), IOSYMBOL("Loudmouth"));
  IoObject *result = IoMessage_locals_performOn_(m, self, self);

  return ISFALSE(result) ? LM_SSL_RESPONSE_STOP : LM_SSL_RESPONSE_CONTINUE;
}

/*** Initialisation functions ***/
IoTag *IoLoudmouth_newTag(void *state) {
  IoTag *tag = IoTag_newWithName_("Loudmouth");

  IoTag_state_(tag, state);
  IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoLoudmouth_free);
  IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoLoudmouth_rawClone);

  return tag;
}

IoLoudmouth *IoLoudmouth_proto(void *state) {
  IoObject *self = IoObject_new(state);
  IoObject_tag_(self, IoLoudmouth_newTag(state));

  IoState_registerProtoWithFunc_(state, self, IoLoudmouth_proto);

  {
    IoMethodTable methodTable[] = {
      {"connect",         IoLoudmouth_connect},
      {"disconnect",      IoLoudmouth_disconnect},
      {"isConnected",     IoLoudmouth_isConnected},
      {"status",          IoLoudmouth_status},

      {"send",            IoLoudmouth_send},
      {"sendRaw",         IoLoudmouth_sendRaw},
      {"updatePresenceAndStatusMsg", IoLoudmouth_setPresence},

      {"startMainLoop",   IoLoudmouth_startMainLoop},
      {"stopMainLoop",    IoLoudmouth_stopMainLoop},
      {"isSslSupported",  IoLoudmouth_isSSLSupported},

      {NULL, NULL},
    };
    IoObject_addMethodTable_(self, methodTable);
  }

  return self;
}

IoLoudmouth *IoLoudmouth_rawClone(IoLoudmouth *proto) {
  IoLoudmouth *self = IoObject_rawClonePrimitive(proto);

  if(main_context == NULL) {
    main_context  = g_main_context_default();
  }

  return self;
}

IoLoudmouth *IoLoudmouth_new(void *state) {
  IoObject *proto = IoState_protoWithInitFunction_((IoState *)state, IoLoudmouth_proto);
  return IOCLONE(proto);
}

void IoLoudmouth_free(IoLoudmouth *self) {
  if(LMCONN(self) != NULL) {
    lm_connection_close(LMCONN(self), NULL);
    lm_connection_unref(LMCONN(self));
  }
}

/*** Loudmouth slots ***/
//doc Loudmouth startMainLoop Starts GMainLoop. Should be called after a connection is established with the server.
IoObject *IoLoudmouth_startMainLoop(IoLoudmouth *self, IoObject *locals, IoMessage *m) {
  main_loop = g_main_loop_new(main_context, FALSE);
  g_main_loop_run(main_loop);
  return self;
}

//doc Loudmouth stopMainLoop
IoObject *IoLoudmouth_stopMainLoop(IoLoudmouth *self, IoObject *locals, IoMessage *m) {
  g_main_loop_quit(main_loop);
  return self;
}

//doc Loudmouth isSslSupported
IoObject *IoLoudmouth_isSSLSupported(IoLoudmouth *self, IoObject *locals, IoMessage *m) {
  return IOBOOL(self, lm_ssl_is_supported());
}

//doc Loudmouth registerAccount(server, username, password) Registers a new account at XMPP server. Returns <code>true</code> or <code>false</code>.
IoObject *IoLoudmouth_registerAccount(IoLoudmouth *self, IoObject *locals, IoMessage *m) {
  char *server    = IoMessage_locals_cStringArgAt_(m, locals, 0),
       *username  = IoMessage_locals_cStringArgAt_(m, locals, 1),
       *password  = IoMessage_locals_cStringArgAt_(m, locals, 2),
       *error_message = "Unknown error";
  LmConnection *connection = lm_connection_new(server);
  LmMessage *xmpp_msg, *xmpp_reply;
  LmMessageNode *query, *node;
  int success = 0;

  if(!lm_connection_open_and_block(connection, NULL)) {
    error_message = "Could not open connection";
    success = 0;
  } else {
    xmpp_msg = lm_message_new_with_sub_type(NULL, LM_MESSAGE_TYPE_IQ, LM_MESSAGE_SUB_TYPE_SET);
    query = lm_message_node_add_child(xmpp_msg->node, "query", NULL);
    lm_message_node_set_attributes(query,
      "xmlns", "jabber:iq:register",
      NULL);
    lm_message_node_add_child(query, "username", username);
    lm_message_node_add_child(query, "password", password);

    xmpp_reply = lm_connection_send_with_reply_and_block(connection, xmpp_msg, NULL);
    lm_connection_close(connection, NULL);
    lm_connection_unref(connection);

    if(!xmpp_reply) {
      success = 0;
      error_message = "No reply from server";
    } else {
      switch(lm_message_get_sub_type(xmpp_reply)) {
        case LM_MESSAGE_SUB_TYPE_RESULT:
          success = 1;
          break;
        case LM_MESSAGE_SUB_TYPE_ERROR:
        default:
          success = 0;
          node = lm_message_node_find_child(xmpp_reply->node, "error");
          error_message = (node == NULL) ? lm_message_node_get_value(node) : "Unknown error";

          lm_message_node_unref(node);
      }
    }

    lm_message_unref(xmpp_reply);
    lm_message_unref(xmpp_msg);
    lm_message_node_unref(query);
  }

  free(server);
  free(username);
  free(password);

  IOASSERT(success, error_message);
  free(error_message);
  return IOBOOL(self, success);
}

//doc Loudmouth connect Connects to the server. Returns <code>self</code>.
IoObject *IoLoudmouth_connect(IoLoudmouth *self, IoObject *locals, IoMessage *m) {
//  Q: Should we io_free() these?
  IoSeq* username   = IoObject_getSlot_(self, IOSYMBOL("username"));
  IoSeq* password   = IoObject_getSlot_(self, IOSYMBOL("password"));
  IoSeq* resource   = IoObject_getSlot_(self, IOSYMBOL("resource"));
  IoSeq* host       = IoObject_getSlot_(self, IOSYMBOL("host"));
  IoNumber* port    = IoObject_getSlot_(self, IOSYMBOL("port"));
  IoObject* use_ssl = IoObject_getSlot_(self, IOSYMBOL("useSsl"));

  IOASSERT(ISSEQ(username), "Loudmouth: username should be a Sequence");
  IOASSERT(ISSEQ(password), "Loudmouth: password should be a Sequence");
  IOASSERT(ISSEQ(resource), "Loudmouth: resource should be a Sequence");
  IOASSERT(ISSEQ(host),     "Loudmouth: host should be a Sequence");
  IOASSERT(ISNUMBER(port),  "Loudmouth: port should be a Number");

  if(LMCONN(self) == NULL) {
    LmConnection *connection = lm_connection_new_with_context(CSTRING(host), main_context);
    IoObject_setDataPointer_(self, connection);

    lm_connection_set_jid(connection, CSTRING(IoObject_getSlot_(self, IOSYMBOL("jid"))));
    lm_connection_set_port(connection, CNUMBER(port));

    if(ISTRUE(use_ssl) && lm_ssl_is_supported()) {
      LmSSL *ssl = lm_ssl_new(NULL, onSslError, NULL, NULL);
      lm_connection_set_ssl(connection, ssl);
      lm_ssl_unref(ssl);
    }

    LmMessageHandler* handler = lm_message_handler_new(onXmppMessage, self, NULL);
    lm_connection_register_message_handler(
      connection, handler,
      LM_MESSAGE_TYPE_MESSAGE, LM_HANDLER_PRIORITY_NORMAL
    );
    lm_message_handler_unref(handler);

    lm_connection_set_disconnect_function(connection, onXmppDisconnect, NULL, NULL);
  }

  lm_connection_open(LMCONN(self), onXmppConnect, self, NULL, NULL);
  return self;
}

//doc Loudmouth disconnect Disconnects from server and returns <code>true</code> if it succeeds.
IoObject *IoLoudmouth_disconnect(IoLoudmouth *self, IoObject *locals, IoMessage *m) {
  return IOBOOL(self, lm_connection_close(LMCONN(self), NULL));
}

//doc Loudmouth isConnected
IoObject *IoLoudmouth_isConnected(IoLoudmouth *self, IoObject *locals, IoMessage *m) {
  return IOBOOL(self, lm_connection_is_authenticated(LMCONN(self)));
}

/*doc Loudmouth status
Returns status connection. Possible return values are:
<ul>
  <li><code>Loudmouth types CLOSED</code></li>
  <li><code>Loudmouth types OPENING</code></li>
  <li><code>Loudmouth types OPEN</code></li>
  <li><code>Loudmouth types AUTHENTICATING</code></li>
  <li><code>Loudmouth types AUTHENTICATED</code></li>
</ul>*/
IoObject *IoLoudmouth_status(IoLoudmouth *self, IoObject *locals, IoMessage *m) {
  IoSeq *status = IOSYMBOL("unknown");
  switch(lm_connection_get_state(LMCONN(self))) {
    case LM_CONNECTION_STATE_CLOSED:         status = IOSYMBOL("closed");         break;
    case LM_CONNECTION_STATE_OPENING:        status = IOSYMBOL("opening");        break;
    case LM_CONNECTION_STATE_OPEN:           status = IOSYMBOL("open");           break;
    case LM_CONNECTION_STATE_AUTHENTICATING: status = IOSYMBOL("authenticating"); break;
    case LM_CONNECTION_STATE_AUTHENTICATED:  status = IOSYMBOL("authenticated");
  }

  return status;
}

//doc Loudmouth send(toJid, message) Sends a message (<code>Sequence</code>) to provided JID (<code>Sequence</code>). Returns <code>true</code> or <code>false</code>.
IoObject *IoLoudmouth_send(IoLoudmouth *self, IoObject *locals, IoMessage *m) {
  char *to        = IoMessage_locals_cStringArgAt_(m, locals, 0);
  char *msg_body  = IoMessage_locals_cStringArgAt_(m, locals, 1);
  int success     = 0;

  LmMessage *xmpp_msg = lm_message_new_with_sub_type(
    to, LM_MESSAGE_TYPE_MESSAGE, LM_MESSAGE_SUB_TYPE_CHAT
  );

  lm_message_node_add_child(xmpp_msg->node, "body", msg_body);
  success = lm_connection_send(LMCONN(self), xmpp_msg, NULL);
  lm_message_unref(xmpp_msg);
  free(to);
  free(msg_body);

  return IOBOOL(self, success);
}

//doc Loudmouth sendRaw(body) Sends raw text over XMPP stream. Returns <code>true</code> if no errors occur.
IoObject *IoLoudmouth_sendRaw(IoLoudmouth *self, IoObject *locals, IoMessage *m) {
  char *seq = IoMessage_locals_cStringArgAt_(m, locals, 0);
  int success = lm_connection_send_raw(LMCONN(self), seq, NULL);
  free(seq);

  return IOBOOL(self, success);
}

IoObject *IoLoudmouth_setPresence(IoLoudmouth *self, IoObject *locals, IoMessage *m) {
  char *pres_c  = IoMessage_locals_cStringArgAt_(m, locals, 0);
  IoSeq *status = IoMessage_locals_valueArgAt_(m, locals, 1);
  int success   = 0;
  LmMessage *xmpp_msg = lm_message_new_with_sub_type(
    NULL,
    LM_MESSAGE_TYPE_PRESENCE,
    str2msg_subtype(pres_c)
  );

  if(ISSEQ(status))
    lm_message_node_add_child(xmpp_msg->node, "status", CSTRING(status));

  success = lm_connection_send(LMCONN(self), xmpp_msg, NULL);
  lm_message_unref(xmpp_msg);
  free(pres_c);

  return IOBOOL(self, success);
}
