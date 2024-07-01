#include <math.h>
#include <ncurses.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "../include/duckwave.h"

float song_duration = 0.0f, song_cursor = 0.0f;



// Função para desenhar gráficos de barras e tempo de reprodução
void draw_bars_and_time(float* data, int count, float current_time, float total_time) {
    clear();
    int max_height = LINES - 2;
    for (int i = 0; i < count; i++) {
        int height = (int)(data[i] * max_height);
        for (int j = 0; j < height; j++) {
            mvprintw(max_height - j, i * 2, "|");
        }
    }

    char time_str[128];
    char total_time_str[128];
    generate_timestamp(current_time, time_str);
    generate_timestamp(total_time, total_time_str);
    mvprintw(LINES - 1, 0, "Time: %s / %s", time_str, total_time_str);

    refresh();
}

// Callback modificado para capturar dados de áudio
void duckwave_playback_callback_with_visual(ma_device* pDevice, void* pOutput, const void* pInput, unsigned int frameCount) {
    ma_decoder* pDecoder = pDevice->pUserData;
    if (pDecoder == NULL)
        return;

    ma_decoder_read_pcm_frames(pDecoder, pOutput, frameCount, NULL);

    // Processar dados de áudio para visualização
    float* audio_data = (float*)pOutput;
    float bar_data[80];
    int samples_per_bar = frameCount / 80;
    for (int i = 0; i < 80; i++) {
        float sum = 0;
        for (int j = 0; j < samples_per_bar; j++) {
            sum += fabs(audio_data[i * samples_per_bar + j]);
        }
        bar_data[i] = sum / samples_per_bar;
    }

    song_cursor += (float)frameCount / pDevice->sampleRate;
    draw_bars_and_time(bar_data, 80, song_cursor, song_duration);

    (void)pInput;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s <audio_file>\n", argv[0]);
        return -1;
    }

    DuckWaveSoundData dw_sdata;
    duckwave_init_file_decoder(&dw_sdata, argv[1]);
    duckwave_init_device_playback(&dw_sdata);

    // Obter a duração da música
    ma_uint64 length_in_pcm_frames;
    ma_result result = ma_decoder_get_length_in_pcm_frames(&dw_sdata.decoder, &length_in_pcm_frames);
    if (result != MA_SUCCESS) {
        printf("Failed to get length of audio file.\n");
        return -1;
    }
    song_duration = (float)length_in_pcm_frames / dw_sdata.decoder.outputSampleRate;

    // Substituir o callback padrão pelo callback com visualização
    dw_sdata.device_config.dataCallback = duckwave_playback_callback_with_visual;

    initscr();
    cbreak();
    noecho();
    curs_set(FALSE);

    result = ma_device_init(NULL, &dw_sdata.device_config, &dw_sdata.device);
    if (result != MA_SUCCESS) {
        printf("Failed to initialize playback device.\n");
        return -1;
    }

    result = ma_device_start(&dw_sdata.device);
    if (result != MA_SUCCESS) {
        printf("Failed to start playback device.\n");
        ma_device_uninit(&dw_sdata.device);
        return -1;
    }

    while (1) {
        // Aguardar até que o usuário pressione uma tecla para sair
        if (getch() != ERR) {
            break;
        }
    }

    ma_device_uninit(&dw_sdata.device);
    endwin();

    return 0;
}