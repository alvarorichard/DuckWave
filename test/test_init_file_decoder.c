#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
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
    struct stat file_stat1, file_stat2;
    int fd;

    printf("Testing with file: %s\n", file);

    if (lstat(file, &file_stat1) == -1) {
        perror("lstat");
        printf("File %s does not exist or cannot be accessed.\n", file);
        return;
    }

    fd = open(file, O_RDONLY);
    if (fd == -1) {
        perror("open");
        printf("Failed to open file %s.\n", file);
        return;
    }

    if (fstat(fd, &file_stat2) == -1) {
        perror("fstat");
        close(fd);
        return;
    }

    // Compare the file attributes to ensure they match
    if (file_stat1.st_dev != file_stat2.st_dev || file_stat1.st_ino != file_stat2.st_ino) {
        printf("File %s has been modified between checks.\n", file);
        close(fd);
        return;
    }

    // Casting away const-ness for duckwave_init_file_decoder
    duckwave_init_file_decoder(&dw_sdata, (char *)file);
    printf("Decoder initialized successfully for file: %s\n", file);
    ma_decoder_uninit(&dw_sdata.decoder);

    close(fd);
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
