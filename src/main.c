

#include <math.h>
#include <ncurses.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <wchar.h>

#include "../include/duckwave.h"

#define NUM_BARS 80
#define SMOOTHING_FACTOR 0.8

float song_duration = 0.0f, song_cursor = 0.0f;
float smoothed_data[NUM_BARS] = {0};
bool song_finished = false;
bool is_paused = false;

DuckWaveSoundData dw_sdata;

void draw_bars_and_time(float* data, int count, float current_time, float total_time) 
{
    clear();
    int max_height = LINES - 3;

    // Caracteres de bloco para criar um efeito de gradiente
    const wchar_t *blocks[] = {L" ", L"░", L"▒", L"▓", L"█"};

    // Cores do gradiente (arco-íris)
    int colors[] = {COLOR_RED, COLOR_YELLOW, COLOR_GREEN, COLOR_CYAN, COLOR_BLUE, COLOR_MAGENTA};

    for (int i = 0; i < count; i++) 
    {
        smoothed_data[i] = SMOOTHING_FACTOR * smoothed_data[i] + (1.0 - SMOOTHING_FACTOR) * data[i];
        int height = (int)(smoothed_data[i] * max_height);
        
        for (int j = 0; j < height; j++) 
        {
            const wchar_t *ch;
            int color_index = (j * (sizeof(colors) / sizeof(int))) / height;
            attron(COLOR_PAIR(color_index + 1));

            if (j < height * 0.2) {
                ch = blocks[1];
            } else if (j < height * 0.4) {
                ch = blocks[2];
            } else if (j < height * 0.6) {
                ch = blocks[3];
            } else {
                ch = blocks[4];
            }
            mvprintw(max_height - j, i * 2, "%ls", ch);
            attroff(COLOR_PAIR(color_index + 1));
        }
    }

    char time_str[128];
    char total_time_str[128];
    generate_timestamp(current_time, time_str);
    generate_timestamp(total_time, total_time_str);
    mvprintw(LINES - 1, 0, "Time: %s / %s", time_str, total_time_str);

    refresh();
}

void duckwave_playback_callback_with_visual(ma_device* pDevice, void* pOutput, const void* pInput, unsigned int frameCount) 
{
    ma_decoder* pDecoder = pDevice->pUserData;
    if (pDecoder == NULL)
        return;

    if (!is_paused) 
    {
        ma_decoder_read_pcm_frames(pDecoder, pOutput, frameCount, NULL);

        float* audio_data = (float*)pOutput;
        float bar_data[NUM_BARS];
        int samples_per_bar = frameCount / NUM_BARS;
        for (int i = 0; i < NUM_BARS; i++) 
        {
            float sum = 0;
            for (int j = 0; j < samples_per_bar; j++) 
            {
                sum += fabsf(audio_data[i * samples_per_bar + j]);
            }
            bar_data[i] = sum / samples_per_bar;
        }

        song_cursor += (float)frameCount / pDevice->sampleRate;
        draw_bars_and_time(bar_data, NUM_BARS, song_cursor, song_duration);

        if (song_cursor >= song_duration) 
        {
            song_finished = true;
            ma_device_stop(pDevice);
        }
    } 
    else 
    {
        memset(pOutput, 0, frameCount * ma_get_bytes_per_frame(pDevice->playback.format, pDevice->playback.channels));
    }

    (void)pInput;
}

void cleanup()
{
    endwin();
    ma_device_uninit(&dw_sdata.device);
    fflush(stdout);
}

int main(int argc, char* argv[]) 
{
    if (argc < 2) 
    {
        printf("Usage: %s <audio_file>\n", argv[0]);
        return -1;
    }

    duckwave_init_file_decoder(&dw_sdata, argv[1]);
    duckwave_init_device_playback(&dw_sdata);

    ma_uint64 length_in_pcm_frames;
    ma_result result = ma_decoder_get_length_in_pcm_frames(&dw_sdata.decoder, &length_in_pcm_frames);
    if (result != MA_SUCCESS) 
    {
        printf("Failed to get length of audio file.\n");
        return -1;
    }
    song_duration = (float)length_in_pcm_frames / dw_sdata.decoder.outputSampleRate;

    dw_sdata.device_config.dataCallback = duckwave_playback_callback_with_visual;

    setlocale(LC_ALL, ""); // Configurar a localidade para UTF-8

    initscr();
    start_color();
    cbreak();
    noecho();
    curs_set(FALSE);
    timeout(0); // Permitir getch() não bloqueante

    atexit(cleanup);

    // Inicializar pares de cores para o gradiente
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);
    init_pair(3, COLOR_GREEN, COLOR_BLACK);
    init_pair(4, COLOR_CYAN, COLOR_BLACK);
    init_pair(5, COLOR_BLUE, COLOR_BLACK);
    init_pair(6, COLOR_MAGENTA, COLOR_BLACK);

    result = ma_device_init(NULL, &dw_sdata.device_config, &dw_sdata.device);
    if (result != MA_SUCCESS) 
    {
        endwin();
        printf("Failed to initialize playback device.\n");
        return -1;
    }

    result = ma_device_start(&dw_sdata.device);
    if (result != MA_SUCCESS) 
    {
        endwin();
        printf("Failed to start playback device.\n");
        ma_device_uninit(&dw_sdata.device);
        return -1;
    }

    while (1) 
    {
        int timeout_ms = (int)((song_duration - song_cursor) * 1000);
        timeout(timeout_ms);

        int ch = getch();
        if (ch != ERR) 
        {
            if (ch == ' ') 
            {
                is_paused = !is_paused;
                if (is_paused) 
                {
                    ma_device_stop(&dw_sdata.device);
                } 
                else 
                {
                    ma_device_start(&dw_sdata.device);
                }
            } 
            else 
            {
                break;
            }
        }

        if (song_finished) 
        {
            break;
        }
    }

    cleanup();

    if (song_finished) 
    {
        printf("Playback finished. Exiting...\n");
    }

    EXIT_SUCCESS;
}
