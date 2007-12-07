/*   Copyright (c) 2002, Steve Dekorte
 *   All rights reserved. See _BSDLicense.txt.
 */

#include "IoAudioTrack.h"
#include "IoObject_actor.h"
#include "base/List.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoBuffer.h"
#include "IoNil.h"


IoObject *IoMessage_locals_audioTrackArgAt_(IoMessage *self, IoObject *locals, int n)
{
  IoObject *v = IoMessage_locals_valueArgAt_(self, locals, n);
  if (!ISTRACK(v)) IoMessage_locals_numberArgAt_errorForType_(self, locals, n, "Track");
  return v;
}

#define DATA(self) ((IoAudioTrackData *)self->data)

IoTag *IoAudioTrack_newTag(void *state)
{
  IoTag *tag = IoTag_newWithName_("Sound");
  IoTag_state_(tag, state);
  IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoAudioTrack_rawClone);
  IoTag_markFunc_(tag, (IoTagMarkFunc *)IoAudioTrack_mark);
  IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoAudioTrack_free);
  return tag;
}

IoAudioTrack *IoAudioTrack_proto(void *state)
{
  IoObject *self = IoObject_new(state);
  IoObject_tag_(self, IoAudioTrack_newTag(state));

  self->data = calloc(1, sizeof(IoAudioTrackData));
  
  DATA(self)->buffer = IoBuffer_newWithCString_(IOSTATE, "");
  DATA(self)->sound = Sound_new();
  Sound_setExternalData_(DATA(self)->sound, IoBuffer_rawUArray(DATA(self)->buffer));
  
  IoState_registerProtoWithFunc_(state, self, IoAudioTrack_proto);

  IoObject_addMethod_(self, IOSTRING("setPath"), IoAudioTrack_setPath);
  IoObject_addMethod_(self, IOSTRING("path"), IoAudioTrack_path);
  IoObject_addMethod_(self, IOSTRING("open"), IoAudioTrack_open);
  IoObject_addMethod_(self, IOSTRING("read"), IoAudioTrack_read);
  IoObject_addMethod_(self, IOSTRING("write"), IoAudioTrack_write);
  
  IoObject_addMethod_(self, IOSTRING("setSampleRate"), IoAudioTrack_setSampleRate);
  IoObject_addMethod_(self, IOSTRING("sampleRate"), IoAudioTrack_sampleRate);
  
  IoObject_addMethod_(self, IOSTRING("setChannels"), IoAudioTrack_setChannels);
  IoObject_addMethod_(self, IOSTRING("channels"), IoAudioTrack_channels);
  
  IoObject_addMethod_(self, IOSTRING("setBuffer"), IoAudioTrack_setBuffer);
  IoObject_addMethod_(self, IOSTRING("buffer"), IoAudioTrack_buffer);

  IoObject_addMethod_(self, IOSTRING("sampleCount"), IoAudioTrack_sampleCount);
  IoObject_addMethod_(self, IOSTRING("seconds"), IoAudioTrack_seconds);
  
  IoObject_addMethod_(self, IOSTRING("setVolume"), IoAudioTrack_setVolume);
  IoObject_addMethod_(self, IOSTRING("volume"), IoAudioTrack_volume);  
  
  IoObject_addMethod_(self, IOSTRING("setChannelVolume"), IoAudioTrack_setChannelVolume);
  IoObject_addMethod_(self, IOSTRING("channelVolume"), IoAudioTrack_channelVolume);  
  
  IoObject_addMethod_(self, IOSTRING("aveOutputCountLength"), IoAudioTrack_aveOutputCountLength);
  IoObject_addMethod_(self, IOSTRING("setAveOutputCountLength"), IoAudioTrack_setAveOutputCountLength);  
  
  IoObject_addMethod_(self, IOSTRING("setPosition"), IoAudioTrack_setPosition);
  IoObject_addMethod_(self, IOSTRING("position"), IoAudioTrack_position);  

  IoObject_addMethod_(self, IOSTRING("setPlayPosition"), IoAudioTrack_setPlayPosition);
  IoObject_addMethod_(self, IOSTRING("playPosition"), IoAudioTrack_playPosition); 
    
  IoObject_addMethod_(self, IOSTRING("isLooping"), IoAudioTrack_isLooping);
  IoObject_addMethod_(self, IOSTRING("loopingOn"), IoAudioTrack_loopingOn);  
  IoObject_addMethod_(self, IOSTRING("loopingOff"), IoAudioTrack_loopingOff);  
  IoObject_addMethod_(self, IOSTRING("buffer"), IoAudioTrack_buffer);  
  IoObject_addMethod_(self, IOSTRING("resampleToSampleCount"), IoAudioTrack_resampleToSampleCount);  
  
  IoObject_addMethod_(self, IOSTRING("setPlayLength"), IoAudioTrack_setPlayLength);  
  IoObject_addMethod_(self, IOSTRING("playLength"), IoAudioTrack_playLength);  
  
  IoObject_addMethod_(self, IOSTRING("pixelsPerSecond"), IoAudioTrack_pixelsPerSecond); 
  
  IoObject_addMethod_(self, IOSTRING("setPixelsPerVolume"), IoAudioTrack_setPixelsPerVolume); 
  IoObject_addMethod_(self, IOSTRING("pixelsPerVolume"), IoAudioTrack_pixelsPerVolume); 
  
  /* --- volumes --- */
  IoObject_addMethod_(self, IOSTRING("setVolumeFromTo"), IoAudioTrack_setVolumeFromTo);  
  IoObject_addMethod_(self, IOSTRING("volumeAt"), IoAudioTrack_volumeAt);  
  IoObject_addMethod_(self, IOSTRING("setSamplesPerVolume"), IoAudioTrack_setSamplesPerVolume);  
  IoObject_addMethod_(self, IOSTRING("samplesPerVolume"), IoAudioTrack_samplesPerVolume); 
  
  IoObject_addMethod_(self, IOSTRING("waveFormDisplayList"), IoAudioTrack_waveFormDisplayList);  
  IoObject_addMethod_(self, IOSTRING("updateWaveFormDisplayList"), IoAudioTrack_updateWaveFormDisplayList);  
  IoObject_addMethod_(self, IOSTRING("volumeMaskDisplayList"), IoAudioTrack_volumeMaskDisplayList);  
  IoObject_addMethod_(self, IOSTRING("updateVolumeMaskDisplayList"), IoAudioTrack_updateVolumeMaskDisplayList);  

  return self;
}

IoAudioTrack *IoAudioTrack_rawClone(IoAudioTrack *self) 
{ 
  IoObject *child = IoObject_rawClonePrimitive(self);
  child->data = cpalloc(self->data, sizeof(IoAudioTrackData));
  DATA(child)->buffer = IoBuffer_rawClone(DATA(self)->buffer);
  DATA(child)->sound = Sound_new();
  Sound_setExternalData_(DATA(child)->sound, IoBuffer_rawUArray(DATA(child)->buffer));
  return child; 
}

IoAudioTrack *IoAudioTrack_new(void *state)
{
  IoObject *proto = IoState_protoWithInitFunction_(state, IoAudioTrack_proto);
  return IoAudioTrack_rawClone(proto);
}

/* ----------------------------------------------------------- */

void IoAudioTrack_free(IoAudioTrack *self) 
{ 
  Sound_free(DATA(self)->sound);
  free(self->data); 
}

void IoAudioTrack_mark(IoAudioTrack *self) 
{
  IoObject_makeGrayIfWhite(DATA(self)->buffer);
}

Sound *IoAudioTrack_rawSound(IoAudioTrack *self) { return DATA(self)->sound; }

/* ----------------------------------------------------------- */

IoObject *IoAudioTrack_setPath(IoAudioTrack *self, IoObject *locals, IoMessage *m)
{ 
  Sound_setPath_(DATA(self)->sound, IoMessage_locals_cStringArgAt_(m, locals, 0));
  return self;
}

IoObject *IoAudioTrack_path(IoAudioTrack *self, IoObject *locals, IoMessage *m)
{ return IOSTRING(Sound_path(DATA(self)->sound)); }

IoObject *IoAudioTrack_open(IoAudioTrack *self, IoObject *locals, IoMessage *m)
{ 
  IoAudioTrack_setPath(self, locals, m); 
  IoAudioTrack_read(self, locals, m); 
  return self;
}

IoObject *IoAudioTrack_read(IoAudioTrack *self, IoObject *locals, IoMessage *m)
{ return Sound_read(DATA(self)->sound) ? self : IONIL(self); }

IoObject *IoAudioTrack_write(IoAudioTrack *self, IoObject *locals, IoMessage *m)
{ return Sound_write(DATA(self)->sound) ? self : IONIL(self); }

IoObject *IoAudioTrack_setSampleRate(IoAudioTrack *self, IoObject *locals, IoMessage *m)
{
  Sound_setSampleRate_(DATA(self)->sound, 
    IoMessage_locals_intArgAt_(m, locals, 0));
  return self;
}

IoObject *IoAudioTrack_sampleRate(IoAudioTrack *self, IoObject *locals, IoMessage *m)
{ return IONUMBER(Sound_sampleRate(DATA(self)->sound)); }

IoObject *IoAudioTrack_setChannels(IoAudioTrack *self, IoObject *locals, IoMessage *m)
{
  Sound_setChannels_(DATA(self)->sound, 
    IoMessage_locals_intArgAt_(m, locals, 0));
  return self;
}

IoObject *IoAudioTrack_channels(IoAudioTrack *self, IoObject *locals, IoMessage *m)
{ return IONUMBER(Sound_channels(DATA(self)->sound)); }

IoObject *IoAudioTrack_setBuffer(IoAudioTrack *self, IoObject *locals, IoMessage *m)
{
  IoBuffer *b = IoMessage_locals_bufferArgAt_(m, locals, 0); 
  Sound_setExternalData_(DATA(self)->sound, IoBuffer_rawUArray(b));
  DATA(self)->buffer = IOREF(b); 
  return self; 
}

IoObject *IoAudioTrack_buffer(IoAudioTrack *self, IoObject *locals, IoMessage *m)
{ return DATA(self)->buffer; }

IoObject *IoAudioTrack_sampleCount(IoAudioTrack *self, IoObject *locals, IoMessage *m)
{ return IONUMBER(Sound_sampleCount(DATA(self)->sound)); }

IoObject *IoAudioTrack_seconds(IoAudioTrack *self, IoObject *locals, IoMessage *m)
{ 
  double d = Sound_seconds(DATA(self)->sound);
  return IONUMBER(d); 
}

IoObject *IoAudioTrack_setVolume(IoAudioTrack *self, IoObject *locals, IoMessage *m)
{
  Sound_setVolume_(DATA(self)->sound, 
    IoMessage_locals_doubleArgAt_(m, locals, 0));
  return self;
}

IoObject *IoAudioTrack_volume(IoAudioTrack *self, IoObject *locals, IoMessage *m)
{ return IONUMBER(Sound_volume(DATA(self)->sound)); }

IoObject *IoAudioTrack_setChannelVolume(IoAudioTrack *self, IoObject *locals, IoMessage *m)
{ 
  int c = IoMessage_locals_intArgAt_(m, locals, 0);
  float f = (float)IoMessage_locals_doubleArgAt_(m, locals, 1);
  if (c == 0) { Sound_setLeftVolume_(DATA(self)->sound, f); }  else
  if (c == 1) { Sound_setRightVolume_(DATA(self)->sound, f); }
  return self; 
}

IoObject *IoAudioTrack_channelVolume(IoAudioTrack *self, IoObject *locals, IoMessage *m)
{ 
  int c = IoMessage_locals_intArgAt_(m, locals, 0);
  if (c == 0) { return IONUMBER(Sound_leftVolume(DATA(self)->sound)); } else
  if (c == 1) { return IONUMBER(Sound_rightVolume(DATA(self)->sound)); }
  return IONIL(self); 
}

IoObject *IoAudioTrack_aveOutput(IoAudioTrack *self, IoObject *locals, IoMessage *m)
{ return IONUMBER(Sound_aveOutput(DATA(self)->sound)); }

IoObject *IoAudioTrack_aveOutputCountLength(IoAudioTrack *self, IoObject *locals, IoMessage *m)
{ return IONUMBER(Sound_aveOutputCountLength(DATA(self)->sound)); }

IoObject *IoAudioTrack_setAveOutputCountLength(IoAudioTrack *self, IoObject *locals, IoMessage *m)
{
  Sound_setAveOutputCountLength_(DATA(self)->sound, 
    IoMessage_locals_intArgAt_(m, locals, 0));
  return self;
}

IoObject *IoAudioTrack_setPosition(IoAudioTrack *self, IoObject *locals, IoMessage *m)
{
  Sound_setPosition_(DATA(self)->sound, 
    IoMessage_locals_intArgAt_(m, locals, 0));
  return self;
}

IoObject *IoAudioTrack_position(IoAudioTrack *self, IoObject *locals, IoMessage *m)
{ return IONUMBER(Sound_position(DATA(self)->sound)); }

IoObject *IoAudioTrack_setPlayPosition(IoAudioTrack *self, IoObject *locals, IoMessage *m)
{
  Sound_setPlayPosition_(DATA(self)->sound, 
    IoMessage_locals_intArgAt_(m, locals, 0));
  return self;
}

IoObject *IoAudioTrack_playPosition(IoAudioTrack *self, IoObject *locals, IoMessage *m)
{ return IONUMBER(Sound_playPosition(DATA(self)->sound)); }

IoObject *IoAudioTrack_isLooping(IoAudioTrack *self, IoObject *locals, IoMessage *m)
{ return IONUMBER(Sound_isLooping(DATA(self)->sound)); }

IoObject *IoAudioTrack_loopingOn(IoAudioTrack *self, IoObject *locals, IoMessage *m)
{ Sound_loopingOn(DATA(self)->sound); return self;}

IoObject *IoAudioTrack_loopingOff(IoAudioTrack *self, IoObject *locals, IoMessage *m)
{ Sound_loopingOff(DATA(self)->sound); return self; }

IoObject *IoAudioTrack_resampleToSampleCount(IoAudioTrack *self, IoObject *locals, IoMessage *m)
{ 
  int sc = IoMessage_locals_intArgAt_(m, locals, 0);
  IOASSERT(sc > 0, "sample count must be greater than 0");
  Sound_resizeToSampleCount_(DATA(self)->sound, sc);
  return self;
}

/* --- playLength --- */

IoObject *IoAudioTrack_setPlayLength(IoAudioTrack *self, IoObject *locals, IoMessage *m)
{ 
  Sound_playLength_(DATA(self)->sound, IoMessage_locals_longArgAt_(m, locals, 0));
  return self;
}

IoObject *IoAudioTrack_playLength(IoAudioTrack *self, IoObject *locals, IoMessage *m)
{ return IONUMBER(Sound_playLength(DATA(self)->sound)); }

/* --- pixelsPerSecond --- */

IoObject *IoAudioTrack_setPixelsPerVolume(IoAudioTrack *self, IoObject *locals, IoMessage *m)
{ 
  Sound_pixelsPerVolume_(DATA(self)->sound, IoMessage_locals_intArgAt_(m, locals, 0));
  return self;
}

IoObject *IoAudioTrack_pixelsPerVolume(IoAudioTrack *self, IoObject *locals, IoMessage *m)
{ return IONUMBER(Sound_pixelsPerVolume(DATA(self)->sound)); }

IoObject *IoAudioTrack_pixelsPerSecond(IoAudioTrack *self, IoObject *locals, IoMessage *m)
{ return IONUMBER(Sound_pixelsPerSecond(DATA(self)->sound)); }

/* --- volumes --- */

IoObject *IoAudioTrack_setVolumeFromTo(IoAudioTrack *self, IoObject *locals, IoMessage *m)
{ 
  int v    = IoMessage_locals_intArgAt_(m, locals, 0);
  int from = IoMessage_locals_intArgAt_(m, locals, 1);
  int to   = IoMessage_locals_intArgAt_(m, locals, 2);
  UArray *ba = Sound_volumes(DATA(self)->sound);
  UArray_setByteWithValue_from_to_(ba, v, from, to);
  return self;
}

IoObject *IoAudioTrack_volumeAt(IoAudioTrack *self, IoObject *locals, IoMessage *m)
{ 
  int i = IoMessage_locals_intArgAt_(m, locals, 0);
  UArray *ba = Sound_volumes(DATA(self)->sound);
  return IONUMBER(UArray_at_(ba, i));
}

IoObject *IoAudioTrack_setSamplesPerVolume(IoAudioTrack *self, IoObject *locals, IoMessage *m)
{ 
  int s = IoMessage_locals_intArgAt_(m, locals, 0);
  Sound_samplesPerVolume_(DATA(self)->sound, s);
  return self;
}

IoObject *IoAudioTrack_samplesPerVolume(IoAudioTrack *self, IoObject *locals, IoMessage *m)
{ 
  int s = Sound_samplesPerVolume(DATA(self)->sound);
  return IONUMBER(s);
}

/* --- pixels per sample --- */


/* --- wave form ------------------------------- */

IoObject *IoAudioTrack_waveFormDisplayList(IoAudioTrack *self, IoObject *locals, IoMessage *m)
{ 
  unsigned int d = Sound_waveFormDisplayList(DATA(self)->sound);
  return IONUMBER(d);
}

IoObject *IoAudioTrack_updateWaveFormDisplayList(IoAudioTrack *self, IoObject *locals, IoMessage *m)
{ 
  Sound_updateWaveFormDisplayList(DATA(self)->sound);
  return self;
}

/* --- volume mask ------------------------------- */

IoObject *IoAudioTrack_volumeMaskDisplayList(IoAudioTrack *self, IoObject *locals, IoMessage *m)
{ 
  unsigned int d = Sound_volumeMaskDisplayList(DATA(self)->sound);
  return IONUMBER(d);
}

IoObject *IoAudioTrack_updateVolumeMaskDisplayList(IoAudioTrack *self, IoObject *locals, IoMessage *m)
{ 
  Sound_updateVolumeMaskDisplayList(DATA(self)->sound);
  return self;
}


