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
 * Function to test the duckwave_init_file_decoder function with a user-provided
 * file.
 *
 * @param file The path to the audio file to be tested.
 */
void test_duckwave_init_file_decoder(const char *file)
{
  DuckWaveSoundData dw_sdata;

  printf("Testing with file: %s\n", file);
  if (access(file, F_OK) != -1) {
    duckwave_init_file_decoder(&dw_sdata, (char *)file);
    printf("Decoder initialized successfully for file: %s\n", file);
    ma_decoder_uninit(&dw_sdata.decoder);
  } else {
    printf("File %s does not exist.\n", file);
  }
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
  test_duckwave_init_file_decoder(audio_file);
  return EXIT_SUCCESS;
}
