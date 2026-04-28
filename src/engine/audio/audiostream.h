#ifndef AUDIOSTREAM_H
#define AUDIOSTREAM_H

#include <AL/al.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define AUDIOSTREAM_BUFFER_SIZE 1024

typedef struct {
	ALuint stream;				// OpenAL stream handle
	ALuint buffer;				// OpenAL buffer handle

	unsigned int sampleRate;    // Frequency (samples per second)
    unsigned int sampleSize;    // Bit depth (bits per sample)
    unsigned int channels;      // Number of channels (1-mono, 2-stereo, ...)
} Audiostream;

void audiostream_init(Audiostream* stream);
void audiostream_deinit(Audiostream* stream);

void audiostream_play(Audiostream* stream);
void audiostream_pause(Audiostream* stream);
void audiostream_rewind(Audiostream *stream);
void audiostream_stop(Audiostream* stream);

bool audiostream_is_ready(Audiostream* stream);
void audiostream_update(Audiostream* stream, void* buf, size_t samples);

#endif
