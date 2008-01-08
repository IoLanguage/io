
#ifndef AudioDevice_DEFINED
#define AudioDevice_DEFINED 1

#include "List.h"
#include "UArray.h"
#include <portaudio.h>

typedef double PaTimestamp;

typedef struct AudioDevice AudioDevice;

typedef void  (AudioDeviceCallback)(void *);

struct AudioDevice
{
	PaError err;
	PaStream *stream;

	int isFreed;
	int locked;
	int needsData;
	AudioDeviceCallback *needsDataCallback;
	void *needsDataCallbackContext;

	/* for speaker output */
	unsigned long writeFrame;
	UArray *writeBuffer;
	UArray *nextWriteBuffer;

	/* for mic input */
	unsigned long readFrame;
	UArray *readBuffer;
	UArray *nextReadBuffer;
	int isListening;
	int maxReadFrame;

	int showed;
	int writeBufferIsEmpty;
	unsigned int lockSleepMicroSeconds;
};

void AudioDevice_nanoSleep(AudioDevice *self);

AudioDevice *AudioDevice_new(void);
void AudioDevice_free(AudioDevice *self);

int AudioDevice_framesPerBuffer(AudioDevice *self);

void AudioDevice_init(AudioDevice *self);
void AudioDevice_terminate(AudioDevice *self);

void AudioDevice_isListening_(AudioDevice *self, int v);
int AudioDevice_isListening(AudioDevice *self);

void AudioDevice_open(AudioDevice *self);
void AudioDevice_openForReadingAndWriting(AudioDevice *self);
void AudioDevice_close(AudioDevice *self);

const char *AudioDevice_error(AudioDevice *self);
void AudioDevice_printError(AudioDevice *self);
void AudioDevice_checkForError(AudioDevice *self);

void AudioDevice_start(AudioDevice *self);
void AudioDevice_stop(AudioDevice *self);

int AudioDevice_isActive(AudioDevice *self);
int AudioDevice_streamTime(AudioDevice *self);
double AudioDevice_cpuLoad(AudioDevice *self);


int AudioDevice_callbackTest(
	void *inputBuffer,
	void *outputBuffer,
	unsigned long framesPerBuffer,
	PaTimestamp outTime,
	void *userData);

int AudioDevice_callbackInputTest(
	void *inputBuffer,
	void *outputBuffer,
	unsigned long framesPerBuffer,
	PaTimestamp outTime,
	void *userData);

int AudioDevice_callback(
	void *inputBuffer,
	void *outputBuffer,
	unsigned long framesPerBuffer,
	PaTimestamp outTime,
	void *userData);


int AudioDevice_swapWriteBuffers(AudioDevice *self);
int AudioDevice_swapReadBuffers(AudioDevice *self);

void AudioDevice_write_(AudioDevice *self, UArray *buf);
void AudioDevice_writeData_length_(AudioDevice *self, uint8_t *data, size_t numBytes);
UArray *AudioDevice_read(AudioDevice *self);
void AudioDevice_clearBuffers(AudioDevice *self);

int AudioDevice_isLocked(AudioDevice *self);

#endif
