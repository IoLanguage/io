#ifndef IoLoudmouth_DEFINED
#define IoLoudmouth_DEFINED 1

#include "IoState.h"
#include "IoObject.h"
#include "IoList.h"
#include "IoNumber.h"
#include "IoSeq.h"

#include <stdlib.h>
#include <string.h>
#include <loudmouth/loudmouth.h>

#define ISLOUDMOUTH(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoLoudmouth_rawClone)

typedef IoObject IoLoudmouth;

// Main context and loop
GMainContext  *main_context;
GMainLoop     *main_loop;

// Helper functions
LmMessageType    str2msg_type(char *str);
LmMessageSubType str2msg_subtype(char *str);

// Loudmouth callbacks
void onXmppConnect(
  LmConnection  *connection,
  int           success,
  void*         data);
void onXmppAuth(
  LmConnection  *connection,
  int           success,
  void*         data);
void onXmppDisconnect(
  LmConnection        *connection,
  LmDisconnectReason  reason,
  void*               data);
LmHandlerResult onXmppMessage(
  LmMessageHandler  *handler,
  LmConnection      *connection,
  LmMessage         *m,
  void*             data);
LmSSLResponse onSslError(
  LmSSL *ssl,
  LmSSLStatus status,
  void* data);

// Initialisation functions
IoTag       *IoLoudmouth_newTag   (void *state);
IoLoudmouth *IoLoudmouth_proto    (void *state);
IoLoudmouth *IoLoudmouth_rawClone (IoLoudmouth *proto);
IoLoudmouth *IoLoudmouth_new      (void *state);
void        IoLoudmouth_free      (IoLoudmouth *self);

// Loudmouth slots
IoObject *IoLoudmouth_startMainLoop   (IoLoudmouth *self, IoObject *locals, IoMessage *m);
IoObject *IoLoudmouth_stopMainLoop    (IoLoudmouth *self, IoObject *locals, IoMessage *m);
IoObject *IoLoudmouth_isSSLSupported  (IoLoudmouth *self, IoObject *locals, IoMessage *m);
IoObject *IoLoudmouth_connect         (IoLoudmouth *self, IoObject *locals, IoMessage *m);
IoObject *IoLoudmouth_disconnect      (IoLoudmouth *self, IoObject *locals, IoMessage *m);
IoObject *IoLoudmouth_isConnected     (IoLoudmouth *self, IoObject *locals, IoMessage *m);
IoObject *IoLoudmouth_status          (IoLoudmouth *self, IoObject *locals, IoMessage *m);
IoObject *IoLoudmouth_send            (IoLoudmouth *self, IoObject *locals, IoMessage *m);
IoObject *IoLoudmouth_sendRaw         (IoLoudmouth *self, IoObject *locals, IoMessage *m);
IoObject *IoLoudmouth_setPresence     (IoLoudmouth *self, IoObject *locals, IoMessage *m);
IoObject *IoLoudmouth_registerAccount (IoLoudmouth *self, IoObject *locals, IoMessage *m);

#endif
