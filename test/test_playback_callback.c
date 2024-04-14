#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "../include/duckwave.h"
#include "../include/miniaudio.h"

// para rodar e executar gcc -o audio_test test_playback_callback.c ../src/duckwave.c -Iinclude -lcmocka -lm              
// para instalar o cmocka sudo apt install libcmocka-dev

// Mock para ma_decoder_read_pcm_frames
static ma_result mock_ma_decoder_read_pcm_frames(ma_decoder* pDecoder, void* pOutput, ma_uint64 frameCount, ma_uint64* pFramesRead) {
    check_expected_ptr(pDecoder);
    check_expected_ptr(pOutput);
    check_expected(frameCount);

    // Simula a leitura de todos os frames solicitados
    if (pFramesRead != NULL) {
        *pFramesRead = frameCount;
    }
    return MA_SUCCESS;
}

// Teste quando pDecoder é NULL
static void test_null_decoder(void **state) {
    ma_device device = {0};
    device.pUserData = NULL; // pDecoder é NULL
    duckwave_default_playback_callback(&device, NULL, NULL, 512);
    // Verifica se a função retorna imediatamente, sem erros
}

// Teste com um pDecoder válido
static void test_valid_decoder(void **state) {
    ma_decoder decoder;
    ma_device device = {0};
    device.pUserData = &decoder;

    // Configura expectativas para o mock
    expect_any(mock_ma_decoder_read_pcm_frames, pDecoder);
    expect_any(mock_ma_decoder_read_pcm_frames, pOutput);
    expect_value(mock_ma_decoder_read_pcm_frames, frameCount, 512);

    duckwave_default_playback_callback(&device, NULL, NULL, 512);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_null_decoder),
        cmocka_unit_test(test_valid_decoder),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}