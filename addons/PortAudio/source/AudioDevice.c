
#include "AudioDevice.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <time.h>
#include <unistd.h>

#define FRAMES_PER_BUFFER 256
#define INPUT_CHANNELS 1

void AudioDevice_nanoSleep(AudioDevice *self)
{
	/*
	 struct timespec rqtp;  
	 struct timespec extra;
	 rqtp.tv_sec = 0;
	 rqtp.tv_nsec = 10; 
	 nanosleep(&rqtp, &extra);
	 */
	usleep(self->lockSleepMicroSeconds);
	//printf("AudioDevice usleep(%i)\n", self->lockSleepMicroSeconds);
}

AudioDevice *AudioDevice_new(void)
{
	AudioDevice *self = calloc(1, sizeof(AudioDevice));
	
	self->writeBuffer = UArray_new();
	self->nextWriteBuffer = UArray_new();
	
	self->readBuffer = UArray_new();
	self->nextReadBuffer = UArray_new();
	self->maxReadFrame = 4096 * 100;
	self->lockSleepMicroSeconds = 10;
	self->needsData = 1;
	
	AudioDevice_init(self);
	return self; 
}

inline void AudioDevice_lock(AudioDevice *self)
{
	while (self->locked) { AudioDevice_nanoSleep(self); }
	self->locked = 1;
}

inline void AudioDevice_unlock(AudioDevice *self)
{
	self->locked = 0;
}

int AudioDevice_isLocked(AudioDevice *self)
{
	return self->locked;
}

void AudioDevice_free(AudioDevice *self)
{
	self->isFreed = 1;
	AudioDevice_lock(self);
	/*
	 while (self->locked) 
	 { printf("AudioDevice_free waiting on lock %i\n", self->locked); }
	 */
	AudioDevice_terminate(self);
	
	UArray_free(self->writeBuffer);
	UArray_free(self->nextWriteBuffer);
	
	UArray_free(self->readBuffer);
	UArray_free(self->nextReadBuffer);
	
	free(self);
}

void AudioDevice_init(AudioDevice *self)
{
	self->locked = 0;
	/*printf("AudioDevice_init\n");*/
	self->err = Pa_Initialize();
	AudioDevice_checkForError(self);
}

int AudioDevice_framesPerBuffer(AudioDevice *self)
{ 
	return FRAMES_PER_BUFFER; 
}

void AudioDevice_terminate(AudioDevice *self)
{
	Pa_Terminate();
}

void AudioDevice_isListening_(AudioDevice *self, int v)
{ 
	self->isListening = v; 
}

int AudioDevice_isListening(AudioDevice *self)
{ 
	return self->isListening; 
}

void AudioDevice_openForReadingAndWriting(AudioDevice *self)
{
	AudioDevice_isListening_(self, 1);
	AudioDevice_open(self);
}

void AudioDevice_open(AudioDevice *self)
{
	if (!self->stream)
	{
		self->err = Pa_OpenDefaultStream(
								   &(self->stream),
								   self->isListening ? INPUT_CHANNELS : 0, /* input channels */
								   2, /* stereo output */
								   paFloat32, 
								   44100, /* sample rate */
								   FRAMES_PER_BUFFER, /* frames per buffer */ 
								   //0, /* number of buffers, if zero then use default minimum */ 
								   /*AudioDevice_callbackInputTest, */
								   AudioDevice_callback, 
								   (void *)self 
								   ); /* pass our data through to callback */
		AudioDevice_checkForError(self);
		//printf("opening self->needsData = 1\n");
		self->needsData = 1;
	}
}

inline int AudioDevice_isOpen(AudioDevice *self) 
{
	return (self->stream != NULL);
}

void AudioDevice_close(AudioDevice *self)
{
	if (AudioDevice_isOpen(self))
	{
		AudioDevice_clearBuffers(self);
		self->err = Pa_CloseStream( self->stream );
		self->stream = NULL;
		AudioDevice_checkForError(self);
	}
}

void AudioDevice_checkForError(AudioDevice *self)
{ 
	if (self->err != paNoError) 
	{ 
		AudioDevice_printError(self); 
	}
}

const char *AudioDevice_error(AudioDevice *self)
{ 
	return self->err ? Pa_GetErrorText(self->err) : NULL;
}

void AudioDevice_printError(AudioDevice *self)
{ 
	printf("AudioDevice error: %s\n", AudioDevice_error(self)); 
}

void AudioDevice_start(AudioDevice *self)
{  
	if (!AudioDevice_isOpen(self)) 
	{
		AudioDevice_open(self);
	}
	
	if (AudioDevice_isOpen(self) && !AudioDevice_isActive(self))
	{
		self->err = Pa_StartStream( self->stream );
		AudioDevice_checkForError(self);
	}
}

void AudioDevice_stop(AudioDevice *self)
{
	if (AudioDevice_isOpen(self) && AudioDevice_isActive(self))
	{
		self->err = Pa_StopStream( self->stream );
		AudioDevice_checkForError(self);
	}
}

int AudioDevice_isActive(AudioDevice *self)
{ 
	return AudioDevice_isOpen(self) ? Pa_IsStreamActive( self->stream ) : 0; 
}

int AudioDevice_streamTime(AudioDevice *self)
{ 
	return AudioDevice_isOpen(self) ? Pa_GetStreamTime( self->stream ) : 0; 
}

double AudioDevice_cpuLoad(AudioDevice *self)
{ 
	return AudioDevice_isOpen(self) ? Pa_GetStreamCpuLoad( self->stream ) : 0; 
}

int AudioDevice_callbackTest(
					    void *inputBuffer, 
					    void *outputBuffer,
					    unsigned long framesPerBuffer,
					    PaTimestamp outTime, 
					    void *userData)
{
	unsigned long frame;
	float *out = (float *)outputBuffer;
	
	for (frame = 0; frame < framesPerBuffer; frame++)
	{
		double k = frame + outTime;
		float a = sin((k * 4 * 261.6) / 44100.0);
		out[frame*2+0] = a;
		out[frame*2+1] = 0.0;
	}
	
	return 0;
}

int AudioDevice_callbackInputTest(
						    void *inputBuffer, 
						    void *outputBuffer,
						    unsigned long framesPerBuffer,
						    PaTimestamp outTime, 
						    void *userData)
{
	unsigned long frame;
	float *in  = (float *)inputBuffer;
	float *out = (float *)outputBuffer;
	
	for (frame = 0; frame < framesPerBuffer; frame++)
	{
		out[frame*2+0] = in[frame];
		out[frame*2+1] = in[frame];
	}
	
	return 0;
}

unsigned long AudioDevice_bytesPerFrame(AudioDevice *self)
{ 
	return sizeof(float)*2; 
}

unsigned long AudioDevice_framesInWriteBuffer(AudioDevice *self)
{
	return UArray_size(self->writeBuffer)/AudioDevice_bytesPerFrame(self);
}

int AudioDevice_callback(
					void *inputBuffer, 
					void *outputBuffer,
					unsigned long framesPerBuffer,
					PaTimestamp outTime, 
					void *userData)
{
	AudioDevice *self = (AudioDevice *)userData;
	self->writeBufferIsEmpty = 0;
	
	//printf("AudioDevice_callback\n");
	
	if (self->isFreed) 
	{ 
		printf("AudioDevice: auto stop portaudio stream\n"); 
		return -1; 
	} /* return non zero to stop stream */

    memset(outputBuffer, 0, framesPerBuffer*2*4);

    AudioDevice_lock(self);
    
    /* --- speaker output ----------------------- */

    if (UArray_size(self->writeBuffer) == 0)
    { 
	    AudioDevice_swapWriteBuffers(self); 
    }

    if (UArray_size(self->writeBuffer))
    {
	    float *out = (float *)outputBuffer;
	    /*int writeFrames = AudioDevice_framesInWriteBuffer(self);*/
	    float *buf  = (float *)UArray_bytes(self->writeBuffer);
	    int outFrame = 0;
	    
	    for (;;)
	    {
		    int outFramesLeft = framesPerBuffer - outFrame;
		    int writeFramesLeft = AudioDevice_framesInWriteBuffer(self) - self->writeFrame;
		    //printf("outFramesLeft = %i\n", outFramesLeft);
		    
		    if (writeFramesLeft < outFramesLeft) /* out > in */
		    {
			    memcpy(out + (outFrame*2), buf + (self->writeFrame*2), writeFramesLeft*2*4);
			    
			    AudioDevice_swapWriteBuffers(self);
			    buf = (float *)UArray_bytes(self->writeBuffer);
			    outFrame += writeFramesLeft;
			    
			    if (AudioDevice_framesInWriteBuffer(self) == 0) 
			    { 
				    //memset(out + (outFrame*2), 0, outFramesLeft*2*4); 
				    //printf("AudioDevice warning: empty buffer ---------------------------------------\n");
				    self->writeBufferIsEmpty = 1;
				    break;
			    }
		    } 
		    else /* in > out */
		    {
			    memcpy(out + (outFrame*2), buf + (self->writeFrame*2), outFramesLeft*2*4);
			    self->writeFrame += outFramesLeft;
			    break;
		    }
	    }
    }
    else
    {
	    self->writeBufferIsEmpty = 1;
    }


    /* --- mic input ----------------------- */

    if (inputBuffer && self->isListening)
    {
	    unsigned long frame = framesPerBuffer;
	    float *input = inputBuffer;
	    float *buf;
	    
	    if (self->readFrame > self->maxReadFrame) 
	    { 
		    self->readFrame = 0; 
	    }
	    
	    UArray_setSize_(self->readBuffer, 
					   (self->readFrame + framesPerBuffer) * sizeof(float) /*bytes */ * 2 /* channels */);
	    
	    buf  = (float *)UArray_bytes(self->readBuffer);
	    
	    buf += self->readFrame * 2;
	    
	    // map mono mic input to stereo output 
	    
	    while (frame)
	    {
		    *buf = *input; buf++;
		    *buf = *input; buf++; 
		    input++;
		    frame--;
	    }
	    self->readFrame += framesPerBuffer;
	    
    }  

    AudioDevice_unlock(self);
    return 0;
}

int AudioDevice_swapWriteBuffers(AudioDevice *self)
{
	/* clear the current buffer */
	UArray_setSize_(self->writeBuffer, 0);
	self->writeFrame = 0;
	
	/* swap if the next one has data */
	if (UArray_size(self->nextWriteBuffer))
	{
		void *b = self->writeBuffer;
		self->writeBuffer = self->nextWriteBuffer;
		self->nextWriteBuffer = b;
		//printf("swapping buffers self->needsData = 1\n");
		self->needsData = 1;
	}
	
	return 0;
}

int AudioDevice_swapReadBuffers(AudioDevice *self)
{
	int didSwap = 0;
	
	if (UArray_size(self->readBuffer))
	{
		/* clear the next buffer */
		UArray_setSize_(self->nextReadBuffer, 0);
		self->readFrame = 0;
		
		/* swap */
		{
			void *b = self->readBuffer;
			self->readBuffer = self->nextReadBuffer;
			self->nextReadBuffer = b;
		}
		didSwap = 1;
	}
	
	return didSwap;
}

void AudioDevice_clearBuffers(AudioDevice *self)
{
	AudioDevice_lock(self);
	UArray_setSize_(self->readBuffer, 0);
	UArray_setSize_(self->nextReadBuffer, 0);
	AudioDevice_unlock(self);
}

/* --- called from the outside --- */

void AudioDevice_writeData_length_(AudioDevice *self, uint8_t *data, size_t numBytes)
{
	self->writeBufferIsEmpty = 0;
	AudioDevice_lock(self);
	UArray_appendBytes_size_(self->nextWriteBuffer, data, numBytes);
	self->needsData = 0;
	//printf("writting self->needsData = 0\n");
	AudioDevice_unlock(self);
	AudioDevice_start(self);
}

void AudioDevice_write_(AudioDevice *self, UArray *buf)
{
	AudioDevice_writeData_length_(self, (unsigned char *)UArray_bytes(buf), UArray_size(buf));
}

UArray *AudioDevice_read(AudioDevice *self)
{
	AudioDevice_start(self);
	AudioDevice_lock(self);
	AudioDevice_swapReadBuffers(self);
	AudioDevice_unlock(self);
	return self->nextReadBuffer;
}
