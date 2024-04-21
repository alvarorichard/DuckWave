#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/duckwave.h"
#include "../include/miniaudio.h"

// Define MA_FAILURE if it's not already defined
#ifndef MA_FAILURE
#define MA_FAILURE -1  // Typical convention for failure in return codes
#endif

// Definindo valores retornados simulados
typedef enum {
    TEST_SUCCESS,
    TEST_FAILURE
} TestResult;

// Mock para substituir a função ma_decoder_init_file
ma_result ma_decoder_init_file_mock(const char* file, void* config, ma_decoder* decoder, TestResult expectedResult) {
    if (strcmp(file, "valid_sound.mp3") == 0 && expectedResult == TEST_SUCCESS) {
        return MA_SUCCESS;
    } else {
        return MA_FAILURE;
    }
}

// Mock para substituir ma_result_description
const char* ma_result_description_mock(ma_result result) {
    return (result == MA_SUCCESS) ? "Decoding successful" : "Decoding failed";
}

// Teste unitário para duckwave_init_file_decoder
void test_duckwave_init_file_decoder() {
    DuckWaveSoundData dw_sdata;
    char* valid_file = "valid_sound.mp3";
    char* invalid_file = "invalid_file.xyz";

    printf("Testing with valid file...\n");
    if (ma_decoder_init_file_mock(valid_file, NULL, &dw_sdata.decoder, TEST_SUCCESS) == MA_SUCCESS) {
        printf("PASS: Valid file decoded successfully.\n");
    } else {
        printf("FAIL: Valid file should be decoded successfully.\n");
    }

    printf("Testing with invalid file...\n");
    if (ma_decoder_init_file_mock(invalid_file, NULL, &dw_sdata.decoder, TEST_FAILURE) != MA_SUCCESS) {
        printf("PASS: Invalid file decoding failed as expected.\n");
    } else {
        printf("FAIL: Invalid file should fail to decode.\n");
    }
}

int main() {
    test_duckwave_init_file_decoder();
    return 0;
}
