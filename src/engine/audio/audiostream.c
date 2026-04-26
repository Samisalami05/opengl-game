#include "audiostream.h"

void audiostream_init(Audiostream* stream) {
	alGenSources(1, &stream->stream);
}

void audiostream_deinit(Audiostream* stream);

void audiostream_play(Audiostream* stream);
void audiostream_pause(Audiostream* stream);
void audiostream_rewind(Audiostream *stream);
void audiostream_stop(Audiostream* stream);

bool audiostream_is_ready(Audiostream* stream);
void Audiostream_update(Audiostream* stream, void* buf, size_t samples);
