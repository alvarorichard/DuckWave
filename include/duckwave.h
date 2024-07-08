#ifndef DUCKWAVE_H
#define DUCKWAVE_H

#include "./miniaudio.h"
#include <stdio.h>
#include <stdlib.h>

/**
 * Given a time in seconds (`float`), this function will write to the reference
 * (`char*`) that same time but in the `hh:mm:ss` format. Expected values
 * could be `72:43:05` or `05:59`, for example.
 */
void generate_timestamp(float, char *);

///  NOTE: Below this function, there should be similar ones; the documentation
///  will be the same. WARN: All functions with `callback` in the name will be
///  executed in a separate thread.

/**
 * These functions are responsible for reading data from the `decoder` object to
 * actually play the music. But **attention**, these functions will be executed
 * in a separate thread, be careful when programming!
 */
void duckwave_default_playback_callback(ma_device *, void *, const void *,
                                        unsigned int);

/**
 * Struct with the objects that miniaudio needs to configure a new device
 * to start playing the specified music.
 */
typedef struct DuckWaveSoundData {
  ma_decoder decoder;
  ma_device_config device_config;
  ma_device device;
} DuckWaveSoundData;

/**
 * Initializes the decoder using a string with the path to the file, maybe not
 * with some formats like MP4.
 */
void duckwave_init_file_decoder(DuckWaveSoundData *, char *);

/**
 * Once the decoder is configured, this function will simply
 * build the device to play the music using the miniaudio playback type.
 */
void duckwave_init_device_playback(DuckWaveSoundData *);

/**
 * Abstraction for the `miniaudio.h` library, it will start the routine to
 * play the music in the background (in a separate thread); checking and
 * handling errors as they appear.
 */
void duckwave_start_playsound_thread(DuckWaveSoundData *);

void test_generate_timestamp();


#endif
