#ifndef WAV_H
#define WAV_H

#include <stdint.h>
#include <stdio.h>

typedef enum {
	WAV_OK,
	WAV_NO_SUCH_FILE,
	WAV_WRONG_FORMAT,
	WAV_EOF,
} WavResult;

typedef struct {
	uint16_t format;
	uint16_t channel_count;
	uint32_t sample_rate; // Number of Samples per second, or Hertz.
	uint32_t byte_per_sec; // Number of bytes to read per second (Frequency * BytePerBloc).
	uint16_t byte_per_block; // Number of bytes per block (NbrChannels * BitsPerSample / 8).
	uint16_t bits_per_sample;
} WavFormat;

typedef struct {
	FILE* file;
	WavFormat format;
	uint32_t size;
	uint32_t data_off;
	uint32_t read_off;
	uint32_t data_size;
} WavFile;

WavResult wav_open(WavFile* wf, char* file_path);
WavResult wav_read(WavFile* wf, uint8_t* buf, uint32_t size);
void wav_close(WavFile* wf);

void wav_print_info(WavFile* wf);

#endif
