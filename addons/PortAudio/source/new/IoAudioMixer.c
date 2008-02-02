//metadoc AudioMixer copyright Steve Dekorte, 2002
//metadoc AudioMixer license All rights reserved. See _BSDLicense.txt.

#include "IoAudioMixer.h"
#include "IoAudioDevice.h"
#include "IoObject_actor.h"
#include "base/List.h"
#include "base/UArray.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoSeq.h"
#include "IoNil.h"
#include "IoSound.h"
#include "IoList.h"
#include <math.h>

#include "SoundTouch_wrapper.h"

/* 1/N of a second worth of frames/samples */
#define FRAMES_PER_BUFFER (44100/32)

/*#define DEBUG*/

AudioEvent *AudioEvent_new(void)
{
	AudioEvent *self = calloc(1, sizeof(AudioEvent));
	return self;
}

AudioEvent *AudioEvent_newWithSound_onSample_of_type_(IoSound *playSound, long sample, IoSound *ioTriggerSound, char etype)
{
	AudioEvent *self = AudioEvent_new();
	self->ioPlaySound = playSound;
	self->sample = sample - 1;
	self->etype = etype;
	if (self->sample < 0) self->sample = 0;
	self->ioTriggerSound = ioTriggerSound;
	return self;
}

void AudioEvent_free(AudioEvent *self)
{
	free(self);
}

void AudioEvent_mark(AudioEvent *self)
{
	if (self->ioTriggerSound) IoObject_makeGrayIfWhite(self->ioTriggerSound);
	if (self->ioPlaySound) IoObject_makeGrayIfWhite(self->ioPlaySound);
}

inline long AudioEvent_samplesUntilTrigger(AudioEvent *self)
{
	if (!self->ioTriggerSound) return 0;
	return self->sample - Sound_position(IoSound_rawSound(self->ioTriggerSound));
}

long AudioEvent_compare(AudioEvent **self, AudioEvent **event)
{ 
	int diff = AudioEvent_samplesUntilTrigger(*event) - AudioEvent_samplesUntilTrigger(*self); 
	if (diff) return diff;
	return ((*self)->etype) == AUDIOEVENT_ADD ? 0 : 1;
}

void AudioEvent_show(AudioEvent *self)
{
	char *type = self->etype == AUDIOEVENT_ADD ? "add" : "remove";
	/*
	 printf("AudioEvent %p %s sound %p at sample %i\n", 
		   (void *)self, type, self->ioPlaySound, (int)self->sample);
	 */
	printf("event %s sound %p\n", type, self->ioPlaySound);
}


/* ---------------------------------------------------------- */

#define DATA(self) ((IoAudioMixerData *)self->data)

IoTag *IoAudioMixer_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("AudioMixer");
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoAudioMixer_rawClone);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoAudioMixer_free);
	IoTag_markFunc_(tag, (IoTagMarkFunc *)IoAudioMixer_mark);
	return tag;
}

IoAudioMixer *IoAudioMixer_proto(void *state)
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoAudioMixer_newTag(state));
	
	self->data = calloc(1, sizeof(IoAudioMixerData));
	DATA(self)->sounds = List_new();
	DATA(self)->soundsToRemove = List_new();
	DATA(self)->events = List_new();
	DATA(self)->activeEvents = List_new();
	DATA(self)->samplesPerBuffer = FRAMES_PER_BUFFER;
	DATA(self)->volume = 1.0;
	
	DATA(self)->ioBuffer = IoSeq_new(IOSTATE);
	DATA(self)->buffer = IoSeq_rawUArray(DATA(self)->ioBuffer);
	DATA(self)->mixBuffer = UArray_new();
	DATA(self)->writeMessage = 
		IoMessage_newWithName_label_(IOSTATE, 
							    IOSYMBOL("write"), 
							    IOSYMBOL("AudioMixer"));
	IoMessage_setCachedArg_to_(DATA(self)->writeMessage, 0, DATA(self)->ioBuffer);
	
	DATA(self)->nonBlockingWriteMessage = 
		IoMessage_newWithName_label_(IOSTATE, 
							    IOSYMBOL("nonBlockingWrite"), 
							    IOSYMBOL("AudioMixer"));
	IoMessage_setCachedArg_to_(DATA(self)->nonBlockingWriteMessage, 0, DATA(self)->ioBuffer);
	
	DATA(self)->soundTouch = SoundTouch_init();
	SoundTouch_setSampleRate(DATA(self)->soundTouch, 44100);
	SoundTouch_setChannels(DATA(self)->soundTouch, 2);
	DATA(self)->tempo = 1.0;
	
	IoState_registerProtoWithFunc_(state, self, IoAudioMixer_proto);
	
	{
		IoMethodTable methodTable[] = {
		{"start", IoAudioMixer_start},
		{"stop", IoAudioMixer_stop},
		{"sounds", IoAudioMixer_sounds},
		{"addSound", IoAudioMixer_addSound_},
		{"addSoundOnSampleOfSound", 
			IoAudioMixer_addSound_onSample_ofSound_},
		{"removeSound", IoAudioMixer_removeSound_},
		{"removeAllSounds", IoAudioMixer_removeAllSounds},
		{"removeSoundOnSampleOfSound", IoAudioMixer_removeSound_onSample_ofSound_},
		{"setSamplesPerBuffer", IoAudioMixer_setSamplesPerBuffer},
		{"setVolume", IoAudioMixer_setVolume},
		{"setTempo", IoAudioMixer_setTempo},
		{"setSampleRate", IoAudioMixer_setSampleRate},
		{"setPitchSemiTones", IoAudioMixer_setPitchSemiTones},
		{"setAudioDevice", IoAudioMixer_setAudioDevice},
		{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}
	
	DATA(self)->ioAudioDevice = IoObject_rawGetSlot_(self, IOSYMBOL("AudioDevice"));
	{
		IoMessage *m = 0x0;
		IOASSERT(DATA(self)->ioAudioDevice, "unable to find AudioDevice");
	}
	return self;
}

IoAudioMixer *IoAudioMixer_rawClone(IoAudioMixer *proto) 
{ 
	IoObject *self = IoObject_rawClonePrimitive(proto);
	self->data = cpalloc(proto->data, sizeof(IoAudioMixerData));
	
	DATA(self)->ioBuffer = IoSeq_new(IOSTATE);
	DATA(self)->buffer = IoSeq_rawUArray(DATA(self)->ioBuffer);
	DATA(proto)->mixBuffer = UArray_new();
	DATA(self)->writeMessage = 
		IoMessage_newWithName_label_(IOSTATE, 
							    IOSYMBOL("write"), 
							    IOSYMBOL("AudioMixer"));
	IoMessage_setCachedArg_to_(DATA(self)->writeMessage, 0, DATA(self)->ioBuffer);
	
	DATA(self)->nonBlockingWriteMessage = 
		IoMessage_newWithName_label_(IOSTATE, 
							    IOSYMBOL("nonBlockingWrite"), 
							    IOSYMBOL("AudioMixer"));
	IoMessage_setCachedArg_to_(DATA(self)->nonBlockingWriteMessage, 0, DATA(self)->ioBuffer);
	
	DATA(self)->sounds = List_new();
	DATA(self)->soundsToRemove = List_new();
	DATA(self)->events = List_new();
	DATA(self)->activeEvents = List_new();
	DATA(self)->volume = DATA(self)->volume;
	
	DATA(self)->soundTouch = SoundTouch_init();
	SoundTouch_setSampleRate(DATA(self)->soundTouch, 44100);
	SoundTouch_setChannels(DATA(self)->soundTouch, 2);
	DATA(self)->tempo = 1.0;
	IoState_addValue_(IOSTATE, self);
	return self; 
}

IoAudioMixer *IoAudioMixer_new(void *state)
{
	IoObject *proto = IoState_protoWithInitFunction_(state, IoAudioMixer_proto);
	return IOCLONE(proto);
}

/* ----------------------------------------------------------- */

void IoAudioMixer_free(IoAudioMixer *self) 
{ 
	List_free(DATA(self)->sounds);
	List_free(DATA(self)->soundsToRemove);
	List_free(DATA(self)->events);
	List_free(DATA(self)->activeEvents);
	UArray_free(DATA(self)->mixBuffer);
	SoundTouch_free(DATA(self)->soundTouch);
	free(self->data); 
}

void IoAudioMixer_mark(IoAudioMixer *self) 
{
	/* buffer is a UArray */
	List_do_(DATA(self)->sounds, (ListDoCallback *)IoObject_makeGrayIfWhite);
	List_do_(DATA(self)->events, (ListDoCallback *)AudioEvent_mark);
	List_do_(DATA(self)->activeEvents, (ListDoCallback *)AudioEvent_mark);
	List_do_(DATA(self)->soundsToRemove, (ListDoCallback *)IoObject_makeGrayIfWhite);
	IoObject_makeGrayIfWhite(DATA(self)->ioAudioDevice);
	IoObject_makeGrayIfWhite(DATA(self)->writeMessage);
	IoObject_makeGrayIfWhite(DATA(self)->nonBlockingWriteMessage);
	IoObject_makeGrayIfWhite(DATA(self)->ioBuffer);
}

/* ----------------------------------------------------------- */

void IoAudioMixer_updateBufferSize(IoAudioMixer *self)
{
	int numBytes = DATA(self)->samplesPerBuffer * sizeof(float) * 2;
	UArray_setSize_(DATA(self)->mixBuffer, numBytes * DATA(self)->tempo);
	UArray_setSize_(DATA(self)->buffer, 10000*8);
}

IoObject *IoAudioMixer_setAudioDevice(IoAudioMixer *self, IoObject *locals, IoMessage *m)
{
	DATA(self)->ioAudioDevice = IOREF(IoMessage_locals_valueArgAt_(m, locals, 0));
	return self;
}

IoObject *IoAudioMixer_setSamplesPerBuffer(IoAudioMixer *self, IoObject *locals, IoMessage *m)
{
	int s = IoMessage_locals_intArgAt_(m, locals, 0);
	DATA(self)->samplesPerBuffer = s;
#ifdef DEBUG
	printf("IoAudioMixer_setSamplesPerBuffer(%i)\n", s);
#endif
	IoAudioMixer_updateBufferSize(self);
	return self;
}

void IoAudioMixer_updateScale(IoAudioMixer *self)
{
	DATA(self)->scale = DATA(self)->volume * (float)(0.9 / sqrt((double)List_size(DATA(self)->sounds))); 
}

IoObject *IoAudioMixer_setVolume(IoAudioMixer *self, IoObject *locals, IoMessage *m)
{
	DATA(self)->volume = IoMessage_locals_doubleArgAt_(m, locals, 0);
	IoAudioMixer_updateScale(self);
	return self;
}

IoObject *IoAudioMixer_sounds(IoAudioMixer *self, IoObject *locals, IoMessage *m)
{
	IoList *ioList = IoList_new(IOSTATE);
	List_copy_(IoList_rawList(ioList), DATA(self)->sounds);
	return ioList;
}

void IoAudioMixer_showEvents(IoAudioMixer *self)
{
	List *events = DATA(self)->events;
	int i;
	for (i=0; i<List_size(events); i++)
	{
		printf("%i: ", i);
		AudioEvent_show(List_at_(events, i));
	}
	/*List_do_(events, (ListDoCallback *)AudioEvent_show);*/
	printf("\n");
}

void IoAudioMixer_addEvent_(IoAudioMixer *self, AudioEvent *event)
{
	List *events = DATA(self)->events;
	List_append_(events, event);
	List_qsort(events, (void *)AudioEvent_compare);
#ifdef DEBUG
	IoAudioMixer_showEvents(self);
#endif
}

IoObject *IoAudioMixer_addSound_(IoAudioMixer *self, IoObject *locals, IoMessage *m)
{
	IoSound *ioSound    = IoMessage_locals_soundArgAt_(m, locals, 0);
	AudioEvent *event = AudioEvent_newWithSound_onSample_of_type_(ioSound, 0, 0x0, AUDIOEVENT_ADD);
	IoAudioMixer_addEvent_(self, event);
	return self;
}

IoObject *IoAudioMixer_addSound_onSample_ofSound_(IoAudioMixer *self, IoObject *locals, IoMessage *m)
{
	IoSound *ioSound        = IoMessage_locals_soundArgAt_(m, locals, 0);
	long sample             = IoMessage_locals_longArgAt_(m, locals, 1);
	IoSound *ioTriggerSound = IoMessage_locals_soundArgAt_(m, locals, 2);
	AudioEvent *event       = AudioEvent_newWithSound_onSample_of_type_(
														   ioSound, sample, ioTriggerSound, AUDIOEVENT_ADD);
	IoAudioMixer_addEvent_(self, event);
	return self;
}

void IoAudioMixer_justAddSound_(IoAudioMixer *self, IoSound *ioSound)
{
	/*printf("add sound %p\n", (void *)ioSound);*/
	List_append_(DATA(self)->sounds, ioSound);
	Sound_setIsPlaying_(IoSound_rawSound(ioSound), 1);
}

void IoAudioMixer_justRemoveSound_(IoAudioMixer *self, IoSound *ioSound)
{
	/*printf("remove sound %p\n", (void *)ioSound);*/
	List_remove_(DATA(self)->sounds, ioSound);
	Sound_setIsPlaying_(IoSound_rawSound(ioSound), 0);
}

IoObject *IoAudioMixer_removeSound_(IoAudioMixer *self, IoObject *locals, IoMessage *m)
{
	IoSound *ioSound = IoMessage_locals_soundArgAt_(m, locals, 0);
	/*List_append_(DATA(self)->soundsToRemove, ioSound);*/
	AudioEvent *event = AudioEvent_newWithSound_onSample_of_type_(ioSound, 0, 0x0, AUDIOEVENT_REMOVE);
	IoAudioMixer_addEvent_(self, event);
	/*IoAudioMixer_justRemoveSound_(self, ioSound);*/
	return self;
}

IoObject *IoAudioMixer_removeAllSounds(IoAudioMixer *self, IoObject *locals, IoMessage *m)
{
	List *sounds = DATA(self)->sounds;
	while (List_size(sounds))
	{ 
		IoAudioMixer_justRemoveSound_(self, List_at_(sounds, 0)); 
	}
	return self;
}


IoObject *IoAudioMixer_removeSound_onSample_ofSound_(IoAudioMixer *self, IoObject *locals, IoMessage *m)
{
	IoSound *ioSound        = IoMessage_locals_soundArgAt_(m, locals, 0);
	long sample             = IoMessage_locals_longArgAt_(m, locals, 1);
	IoSound *ioTriggerSound = IoMessage_locals_soundArgAt_(m, locals, 2);
	AudioEvent *event       = 
		AudioEvent_newWithSound_onSample_of_type_(ioSound, sample, ioTriggerSound, AUDIOEVENT_REMOVE);
	IoAudioMixer_addEvent_(self, event);
	return self;
}

/* ----------------------------------------------------------- */

int IoAudioMixer_isActive(IoAudioMixer *self)
{
	return List_size(DATA(self)->sounds) || 
	List_size(DATA(self)->soundsToRemove) ||
	List_size(DATA(self)->events) ||
	List_size(DATA(self)->activeEvents);
}

IoObject *IoAudioMixer_start(IoAudioMixer *self, IoObject *locals, IoMessage *m)
{
	IoAudioMixer_updateBufferSize(self);
	
	DATA(self)->isRunning = 1;
	while (DATA(self)->isRunning)
	{
		if (IoAudioMixer_isActive(self)) 
		{
			int outSamples = IoAudioMixer_mixOneChunk(self, locals, m);
			if (outSamples < 5000)
			{   
#ifdef DEBUG
				printf("< 5000 out - mix another chunk\n");
#endif
				IoAudioMixer_mixOneChunk(self, locals, m); 
			}
		}
		//IoState_yield(IOSTATE);
	}
	return self;
}

IoObject *IoAudioMixer_stop(IoAudioMixer *self, IoObject *locals, IoMessage *m)
{  
	DATA(self)->isRunning = 0;
	/*IoAudioDevice_clearBuffers(DATA(self)->ioAudioDevice);*/
	return self;
}

void IoAudioMixer_processSoundRemovals(IoAudioMixer *self)
{
	IoSound *s;
	List *soundsToRemove = DATA(self)->soundsToRemove;
	List *sounds = DATA(self)->sounds;
	int removeCount = List_size(soundsToRemove);
	
	if (removeCount) 
	{
		printf("removeCount = %i\n", removeCount); 
		printf("soundsCount = %i\n", List_size(sounds)); 
	}
	
	while ( (s = List_pop(soundsToRemove)) )
	{ IoAudioMixer_justRemoveSound_(self, s); }
	
	if (removeCount) 
	{
		printf("remaining sounds = %i\n", List_size(sounds)); 
		printf("remaining removeCount = %i\n\n", List_size(soundsToRemove)); 
	}
	
	IoAudioMixer_updateScale(self);
}

void IoAudioMixer_processActiveEvents(IoAudioMixer *self)
{
	AudioEvent *e;
	while ( (e = List_pop(DATA(self)->activeEvents)) )
	{ 
#ifdef DEBUG
		printf("processing: ");
		AudioEvent_show(e);
#endif
		switch (e->etype)
		{
			case AUDIOEVENT_ADD:
				IoAudioMixer_justAddSound_(self, e->ioPlaySound);
				break;
			case AUDIOEVENT_REMOVE:
				IoAudioMixer_justRemoveSound_(self, e->ioPlaySound);
				break;
		}
	}
	IoAudioMixer_updateScale(self);
}

int IoAudioMixer_mixOneChunk(IoAudioMixer *self, IoObject *locals, IoMessage *m)
{ 
	UArray *mixBuffer = DATA(self)->mixBuffer;
	List *sounds = DATA(self)->sounds;
	List *soundsToRemove = DATA(self)->soundsToRemove;
	AudioEvent *e = List_top(DATA(self)->events);
	List *activeEvents = DATA(self)->activeEvents;
	int frame;
	/*int samplesPerBuffer = DATA(self)->samplesPerBuffer;*/
	int samplesPerBuffer = UArray_size(DATA(self)->mixBuffer) / (sizeof(float) * 2);
	
	UArray_setAllBytesTo_(mixBuffer, 0);
	
	while ( e && (!e->ioTriggerSound))
	{
		List_append_(DATA(self)->activeEvents, List_pop(DATA(self)->events));
		e = List_top(DATA(self)->events);
	}
	
	if (List_size(activeEvents))   IoAudioMixer_processActiveEvents(self);
	if (List_size(soundsToRemove)) IoAudioMixer_processSoundRemovals(self);
	
	for (frame = 0; frame < samplesPerBuffer; frame ++)
	{
		int index = frame * 2;
		int i;
		float *ol = UArray_floatPointerAt_(mixBuffer, index);
		float *or = UArray_floatPointerAt_(mixBuffer, index+1);
		
		for (i = 0; i < List_size(sounds); i++)
		{
			IoSound *ioSound = List_at_(sounds, i);
			Sound *sound = IoSound_rawSound(ioSound);
			float left, right;
			char done = Sound_nextFloat(sound, &left, &right);
			
			if (done && !Sound_isLooping(sound)) 
			{
				List_append_(soundsToRemove, ioSound);
				continue; 
			}
			
			(*ol) += left;
			(*or) += right;
			
			while (e && 
				  ((!e->ioTriggerSound) || 
				   ((e->ioTriggerSound == ioSound) && 
				    (e->sample == Sound_position(sound))))
				  )
			{
				List_append_(DATA(self)->activeEvents, List_pop(DATA(self)->events));
				e = List_top(DATA(self)->events);
			}
		}
		
		(*ol) *= DATA(self)->scale;
		(*or) *= DATA(self)->scale;
		
		if (List_size(activeEvents))   IoAudioMixer_processActiveEvents(self);
		if (List_size(soundsToRemove)) IoAudioMixer_processSoundRemovals(self);
	}
	
	/* adjust pitch and tempo */
	{
		//double t1 = ((double)clock())/((double)CLOCKS_PER_SEC); 
		//double t2; 
		int receivedSamples = 1;
		
		SoundTouch_putSamples(DATA(self)->soundTouch, (float *)UArray_bytes(mixBuffer), samplesPerBuffer);
		//printf("put %i\n", samplesPerBuffer);
		//while (receivedSamples)
		{
			UArray_setSize_(DATA(self)->buffer, 10000*8);
			receivedSamples = SoundTouch_receiveSamples(DATA(self)->soundTouch, 
											    (float *)UArray_bytes(DATA(self)->buffer), 
											    UArray_size(DATA(self)->buffer) / (sizeof(float) * 2));
			UArray_setSize_(DATA(self)->buffer, receivedSamples * (sizeof(float) * 2));
			
			//printf("received %i\n", receivedSamples);
			if (receivedSamples)
			{
				if (receivedSamples < 5000)
				{
#ifdef DEBUG
					printf("non-blocking write\n");
#endif
					
					IoMessage_locals_performOn_(DATA(self)->nonBlockingWriteMessage, 
										   self, DATA(self)->ioAudioDevice);
				}
				else
				{
					IoMessage_locals_performOn_(DATA(self)->writeMessage, 
										   self, DATA(self)->ioAudioDevice);
				}
			}
#ifdef DEBUG
			t2 = ((double)clock())/((double)CLOCKS_PER_SEC);
			printf("tempo: %1.1f  %i -> %i in %0.2f sec\n", 
				  DATA(self)->tempo, 
				  samplesPerBuffer, 
				  receivedSamples, 
				  (float)(t2 - t1));
#endif
		}
		
		//printf("\n");
		return receivedSamples;
	}
	/* need to change this to be dynamic, so we can easily record the output */
	/*IoAudioDevice_justWrite(DATA(self)->ioAudioDevice, locals, m, buffer);*/
	return UArray_size(DATA(self)->buffer) / 8;
}

IoObject *IoAudioMixer_setTempo(IoAudioMixer *self, IoObject *locals, IoMessage *m)
{
	DATA(self)->tempo = IoMessage_locals_floatArgAt_(m, locals, 0);
	SoundTouch_setTempo(DATA(self)->soundTouch, DATA(self)->tempo);
	printf("IoAudioMixer_setTempo(%f)\n", DATA(self)->tempo); 
	IoAudioMixer_updateBufferSize(self);
	return self;
}

IoObject *IoAudioMixer_setSampleRate(IoAudioMixer *self, IoObject *locals, IoMessage *m)
{
	DATA(self)->sampleRate = IoMessage_locals_intArgAt_(m, locals, 0);
	SoundTouch_setSampleRate(DATA(self)->soundTouch, (unsigned int)DATA(self)->sampleRate);
	printf("IoAudioMixer_setSampleRate(%i)\n", DATA(self)->sampleRate); 
	IoAudioMixer_updateBufferSize(self);
	return self;
}

IoObject *IoAudioMixer_setPitchSemiTones(IoAudioMixer *self, IoObject *locals, IoMessage *m)
{
	DATA(self)->pitch = IoMessage_locals_floatArgAt_(m, locals, 0);
	SoundTouch_setPitchSemiTones(DATA(self)->soundTouch, DATA(self)->pitch);
	printf("IoAudioMixer_setPitchSemiTones(%f)\n", DATA(self)->pitch); 
	IoAudioMixer_updateBufferSize(self);
	return self;
}

