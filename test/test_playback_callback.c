#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MINIAUDIO_IMPLEMENTATION
#include "../include/duckwave.h"
#include "../include/miniaudio.h"

// clang test.f -lpthread -lm

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
 * This function initializes the audio decoder and playback device,
 * reads some audio data for debugging, and starts the playback device
 * to play the audio.
 *
 * @param audio_file The path to the audio file to be played.
 */
void play_audio(const char *audio_file)
{
  ma_result result;
  ma_decoder_config config;
  ma_decoder decoder;
  ma_device_config device_config;
  ma_device device;

  // Configure the decoder explicitly for ma_format_u8
  config = ma_decoder_config_init(ma_format_u8, 2, 48000);

  // Initialize the decoder with a provided audio file
  result = ma_decoder_init_file(audio_file, &config, &decoder);
  if (result != MA_SUCCESS) {
    printf("Failed to initialize decoder: %s\n", ma_result_description(result));
    return;
  }

  printf("Decoder initialized successfully\n");
  printf("Output format: %d\n", decoder.outputFormat);
  printf("Output channels: %d\n", decoder.outputChannels);
  printf("Output sample rate: %d\n", decoder.outputSampleRate);

  // Read some audio data for debugging
  ma_uint64 framesRead;
  ma_uint8 buffer[4096];
  result = ma_decoder_read_pcm_frames(
      &decoder, buffer,
      sizeof(buffer) /
          ma_get_bytes_per_frame(decoder.outputFormat, decoder.outputChannels),
      &framesRead);
  if (result == MA_SUCCESS && framesRead > 0) {
    printf("Frames read: %llu\n", framesRead);

    // Check if the data is all zeros or not
    int all_zeros = 1;
    for (size_t i = 0;
         i < framesRead * ma_get_bytes_per_frame(decoder.outputFormat,
                                                 decoder.outputChannels);
         ++i) {
      if (buffer[i] != 0) {
        all_zeros = 0;
        break;
      }
    }

    if (all_zeros) {
      printf("All bytes are zero\n");
    } else {
      printf("Some bytes are non-zero\n");
    }

    // Print the first bytes of audio data
    printf(
        "First bytes of audio data: %02x %02x %02x %02x %02x %02x %02x "
        "%02x...\n",
        buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5],
        buffer[6], buffer[7]);
  }

  // Configure the audio device
  device_config = ma_device_config_init(ma_device_type_playback);
  device_config.playback.format = decoder.outputFormat;
  device_config.playback.channels = decoder.outputChannels;
  device_config.sampleRate = decoder.outputSampleRate;
  device_config.dataCallback = duckwave_default_playback_callback;
  device_config.pUserData = &decoder;

  // Initialize the audio device
  result = ma_device_init(NULL, &device_config, &device);
  if (result != MA_SUCCESS) {
    printf("Failed to initialize device: %s\n", ma_result_description(result));
    ma_decoder_uninit(&decoder);
    return;
  }

  printf("Audio device initialized successfully\n");

  // Start the audio device to play the audio
  result = ma_device_start(&device);
  if (result != MA_SUCCESS) {
    printf("Failed to start device: %s\n", ma_result_description(result));
    ma_device_uninit(&device);
    ma_decoder_uninit(&decoder);
    return;
  }

  printf("Playing audio... Press Enter to stop.\n");
  getchar(); // Wait for the user to press Enter to stop playback

  // Clean up resources
  ma_device_uninit(&device);
  ma_decoder_uninit(&decoder);
}

/**
 * The main function that initializes the audio playback.
 *
 * @param argc The number of command-line arguments.
 * @param argv The array of command-line arguments.
 * @return EXIT_SUCCESS on successful execution, EXIT_FAILURE otherwise.
 */
int main(int argc, char **argv)
{
  if (argc < 2) {
    printf("Usage: %s <audio_file>\n", argv[0]);
    return EXIT_FAILURE;
  }

  const char *audio_file = argv[1];
  play_audio(audio_file);
  return EXIT_SUCCESS;
}
