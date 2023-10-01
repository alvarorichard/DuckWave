// playmp3.h

#pragma once

#include <mpg123.h>
#include <ao/ao.h>
#include <stddef.h>

typedef struct {
    size_t buffer_size, done;
    int driver, err, channels, encoding;
    long rate;
    char* track;
    mpg123_handle *mh;
    char* buffer;
    ao_sample_format format;
    ao_device *dev;
} PlayMP3;

void init_PlayMP3(PlayMP3* mp3);
void setMusic(PlayMP3* mp3, char* track);
void play(PlayMP3* mp3);
void cleanup_PlayMP3(PlayMP3* mp3);
