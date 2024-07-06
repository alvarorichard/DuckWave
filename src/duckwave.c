#include <stdio.h>
#include <stdlib.h>

#define MINIAUDIO_IMPLEMENTATION

#include "../include/duckwave.h"

/**
 * This function generates a timestamp string in the format hh:mm:ss or mm:ss from a float representing seconds.
 *
 * @param secs The total time in seconds.
 * @param res The resulting formatted timestamp string. It should be large enough to hold the resulting string (128 bytes is assumed here).
 */
void generate_timestamp(float secs, char* res)
{
    int hours = (int)secs / 3600;
    secs = (int)secs % 3600;
    int mins = (int)secs / 60;
    secs = (int)secs % 60;

    if (hours > 0)
        snprintf(res, 128, "%d:%02d:%02.0f", hours, mins, secs); // Format hh:mm:ss
    else
        snprintf(res, 128, "%02d:%02.0f", mins, secs); // Format mm:ss
}

/**
 * This is the default playback callback function for miniaudio, which is called periodically to provide audio data for playback.
 *
 * @param pDevice The audio device that is calling this callback.
 * @param pOutput Pointer to the buffer where audio data should be written.
 * @param _pInput Pointer to the buffer containing input audio data (unused here).
 * @param frameCount The number of audio frames to be processed.
 */
void duckwave_default_playback_callback(ma_device* pDevice, void* pOutput, const void* _pInput, unsigned int frameCount)
{
    ma_decoder *pDecoder = pDevice->pUserData;

    if (pDecoder == NULL)
        return;

    ma_decoder_read_pcm_frames(pDecoder, pOutput, frameCount, NULL);

    (void)_pInput;
}

/**
 * Initializes the file decoder using a given file path.
 *
 * @param dw_sdata Pointer to the DuckWaveSoundData structure which holds the decoder and device configuration.
 * @param file Path to the audio file to be decoded.
 */
void duckwave_init_file_decoder(DuckWaveSoundData* dw_sdata, char* file)
{
    ma_result result;

    result = ma_decoder_init_file(file, NULL, &dw_sdata->decoder);

    if (result != MA_SUCCESS) {
        printf("\nCould not decode current %s file.\n", file);
        printf("%s.\n\n", ma_result_description(result));

        exit(EXIT_FAILURE);
    }
}

/**
 * Initializes the playback device configuration based on the decoder's output format, channels, and sample rate.
 *
 * @param dw_sdata Pointer to the DuckWaveSoundData structure which holds the decoder and device configuration.
 */
void duckwave_init_device_playback(DuckWaveSoundData* dw_sdata)
{
    ma_decoder *decoder = &dw_sdata->decoder;
    ma_device_config *device_config = &dw_sdata->device_config;

    *device_config = ma_device_config_init(ma_device_type_playback);

    device_config->playback.format = decoder->outputFormat;
    device_config->playback.channels = decoder->outputChannels;
    device_config->sampleRate = decoder->outputSampleRate;
    device_config->dataCallback = duckwave_default_playback_callback;
    device_config->pUserData = decoder;
}

/**
 * Starts the audio playback on a separate thread using the initialized device and decoder.
 *
 * @param dw_sdata Pointer to the DuckWaveSoundData structure which holds the decoder and device configuration.
 */
void duckwave_start_playsound_thread(DuckWaveSoundData* dw_sdata)
{
    ma_result result;

    ma_decoder *decoder = &dw_sdata->decoder;
    ma_device *device = &dw_sdata->device;
    ma_device_config *device_config = &dw_sdata->device_config;

    result = ma_device_init(NULL, device_config, device);

    if (result != MA_SUCCESS) {
        printf("\nCannot initialize device.\n");
        printf("%s.\n\n", ma_result_description(result));

        ma_decoder_uninit(decoder);

        exit(EXIT_FAILURE);
    }

    result = ma_device_start(device);

    if (result != MA_SUCCESS) {
        printf("\nCannot start device.\n");
        printf("%s.\n\n", ma_result_description(result));

        ma_device_uninit(device);
        ma_decoder_uninit(decoder);

        exit(EXIT_FAILURE);
    }
}
