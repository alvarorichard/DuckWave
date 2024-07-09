#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // For access function
#define MINIAUDIO_IMPLEMENTATION
#include "../include/duckwave.h"
#include "../include/miniaudio.h"

// clang test.f -lpthread -lm

/**
 * This function initializes the file decoder using a given file path.
 *
 * @param dw_sdata Pointer to the DuckWaveSoundData structure which holds the
 * decoder and device configuration.
 * @param file Path to the audio file to be decoded.
 */
void duckwave_init_file_decoder(DuckWaveSoundData *dw_sdata, char *file)
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
 * This function initializes the playback device configuration based on the
 * decoder's output format, channels, and sample rate.
 *
 * @param dw_sdata Pointer to the DuckWaveSoundData structure which holds the
 * decoder and device configuration.
 */
void duckwave_init_device_playback(DuckWaveSoundData *dw_sdata)
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
 * This is the default playback callback function for miniaudio, which is called
 * periodically to provide audio data for playback.
 *
 * @param pDevice The audio device that is calling this callback.
 * @param pOutput Pointer to the buffer where audio data should be written.
 * @param _pInput Pointer to the buffer containing input audio data (unused
 * here).
 * @param frameCount The number of audio frames to be processed.
 */
void duckwave_default_playback_callback(ma_device *pDevice, void *pOutput,
                                        const void *_pInput,
                                        unsigned int frameCount)
{
  ma_decoder *pDecoder = pDevice->pUserData;
  if (pDecoder == NULL) {
    printf("Decoder is NULL\n");
    return;
  }

  ma_uint64 framesRead;
  ma_result result =
      ma_decoder_read_pcm_frames(pDecoder, pOutput, frameCount, &framesRead);

  if (result != MA_SUCCESS) {
    printf("Failed to read PCM frames: %s\n", ma_result_description(result));
  } else {
    printf("Frames read: %llu / %u\n", framesRead, frameCount);
  }

  if (framesRead < frameCount) {
    // Zero out the rest of the buffer to avoid noise
    memset((unsigned char *)pOutput +
               framesRead * ma_get_bytes_per_frame(pDevice->playback.format,
                                                   pDevice->playback.channels),
           0,
           (frameCount - framesRead) *
               ma_get_bytes_per_frame(pDevice->playback.format,
                                      pDevice->playback.channels));
  }

  (void)_pInput;
}

/**
 * This function initializes and starts the playback device.
 *
 * @param dw_sdata Pointer to the DuckWaveSoundData structure which holds the
 * decoder and device configuration.
 */
void duckwave_start_playsound_thread(DuckWaveSoundData *dw_sdata)
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

/**
 * Function to test the duckwave_start_playsound_thread function.
 *
 * @param file The path to the audio file to be tested.
 */
void test_duckwave_start_playsound_thread(const char *file)
{
  DuckWaveSoundData dw_sdata;

  // Initialize the decoder with the provided file
  printf("Testing with file: %s\n", file);
  if (access(file, F_OK) != -1) {
    duckwave_init_file_decoder(&dw_sdata, (char *)file);
    printf("Decoder initialized successfully for file: %s\n", file);

    // Initialize the device playback configuration
    duckwave_init_device_playback(&dw_sdata);
    printf("Device playback configuration initialized.\n");

    // Start the playback device
    duckwave_start_playsound_thread(&dw_sdata);
    printf("Playback device started successfully.\n");

    // Wait for a short duration to simulate playback
    printf("Simulating playback for 5 seconds...\n");
    sleep(5);

    // Cleanup
    ma_device_uninit(&dw_sdata.device);
    ma_decoder_uninit(&dw_sdata.decoder);
    printf("Resources cleaned up successfully.\n");
  } else {
    printf("File %s does not exist.\n", file);
  }
}

int main(int argc, char **argv)
{
  if (argc < 2) {
    printf("Usage: %s <audio_file>\n", argv[0]);
    return EXIT_FAILURE;
  }

  const char *audio_file = argv[1];
  test_duckwave_start_playsound_thread(audio_file);
  return EXIT_SUCCESS;
}
