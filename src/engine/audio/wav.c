#include "wav.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static WavResult wav_expect(WavFile* wf, uint8_t* data) {
	uint8_t buf[4];
	if (fread(buf, 4, 1, wf->file) != 1) return WAV_EOF;

	return memcmp(buf, data, 4) == 0 ? WAV_OK : WAV_WRONG_FORMAT;
}

static WavResult wav_find_data(WavFile* wf) {
	char name[4] = {0};
	while (fread(name, 4, 1, wf->file) == 1 && memcmp(name, "data", 4) != 0) {
		uint32_t size = 0;
		if (fread(&size, 4, 1, wf->file) != 1) return WAV_EOF;
		fseek(wf->file, size, SEEK_CUR);
		wf->data_off += 8 + size;
	}

	if (memcmp(name, "data", 4) != 0) return WAV_WRONG_FORMAT;

	wf->data_off += 8;
	if (fread(&wf->data_size, 4, 1, wf->file) != 1) return WAV_EOF;
	return WAV_OK;
}

WavResult wav_open(WavFile* wf, char* file_path) {
	wf->file = fopen(file_path, "rb");
	if (wf->file == NULL) return WAV_NO_SUCH_FILE;
	
	WavResult res = WAV_OK;
	if ((res = wav_expect(wf, (uint8_t*)"RIFF")) != WAV_OK) return res;
	if (fread(&wf->size, 4, 1, wf->file) != 1) return WAV_EOF;
	if ((res = wav_expect(wf, (uint8_t*)"WAVE")) != WAV_OK) return res;

	if ((res = wav_expect(wf, (uint8_t*)"fmt ")) != WAV_OK) return res;
	uint32_t expected_size = 16;
	if ((res = wav_expect(wf, (uint8_t*)&expected_size)) != WAV_OK) return res;


	if (fread(&wf->format, 16, 1, wf->file) != 1) return WAV_EOF;
	
	wf->data_off = 36;
	if ((res = wav_find_data(wf)) != WAV_OK) return res;
	wf->read_off = wf->data_off;

	return WAV_OK;
}

WavResult wav_read(WavFile* wf, uint8_t* buf, uint32_t size) {
	if (wf->read_off + size > wf->data_off + wf->data_size) return WAV_EOF;
	
	if (fread(buf, size, 1, wf->file) != 1) return WAV_EOF;
	return WAV_OK;
}

void wav_close(WavFile* wf) {
	fclose(wf->file);
	*wf = (WavFile){0};
}

void wav_print_info(WavFile* wf) {
	printf(" --- WAVE FILE --- \n");
	printf("file size: %d\n\n", wf->size);

	printf("[FORMAT]\n");
	printf("format: %d\n", wf->format.format);
	printf("num channels: %d\n", wf->format.channel_count);
	printf("sample rate: %d\n", wf->format.sample_rate);
	printf("byte per second: %d\n", wf->format.byte_per_sec);
	printf("byte per block: %d\n", wf->format.byte_per_block);
	printf("bits per sample: %d\n\n", wf->format.bits_per_sample);

	printf("[DATA]\n");
	printf("data offset: %d\n", wf->data_off);
	printf("data size: %d\n\n", wf->data_size);
}
