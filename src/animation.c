
// File: src/animation.c
#include <locale.h>
#include <math.h>
#include <ncurses.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#include "../include/duckwave.h"
#include "../include/animation.h"

#define NUM_BARS 80
#define SMOOTHING_FACTOR 0.8

float smoothed_data[NUM_BARS] = {0};

/**
 * @file animation.c
 * @brief Animation functions for visualizing audio playback.
 */

/**
 * @brief Draws bars representing audio data and displays the current playback
 * time.
 *
 * This function clears the screen and draws a bar graph to visualize the audio
 * data. It uses smoothed data to create a smoother animation effect. The bars
 * are colored with a gradient effect, and the current playback time and total
 * duration are displayed.
 *
 * @param data An array of floating-point values representing the audio data to
 * be visualized.
 * @param count The number of bars to draw, corresponding to the number of
 * elements in the data array.
 * @param current_time The current playback time in seconds.
 * @param total_time The total duration of the audio track in seconds.
 *
 * The function uses the ncurses library to draw the bars and display the time.
 * The bars are drawn using a gradient of block characters and colors.
 */

void draw_bars_and_time(float* data, int count, float current_time,
                        float total_time)
{
  clear();
  int max_height = LINES - 3;

  const wchar_t* blocks[] = {L" ", L"░", L"▒", L"▓", L"█"};
  int colors[] = {COLOR_RED,  COLOR_YELLOW, COLOR_GREEN,
                  COLOR_CYAN, COLOR_BLUE,   COLOR_MAGENTA};

  for (int i = 0; i < count; i++) {
    smoothed_data[i] = SMOOTHING_FACTOR * smoothed_data[i] +
                        (1.0 - SMOOTHING_FACTOR) * data[i];
    int height = (int)(smoothed_data[i] * max_height);

    for (int j = 0; j < height; j++) {
      const wchar_t* ch;
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
/**
 * @brief Callback function for audio playback with visual representation.
 *
 * This function is a callback for audio playback using the MiniAudio library.
 * It reads PCM frames from the audio decoder, processes the audio data to
 * calculate bar heights for visualization, and updates the visual display. It
 * also handles the end of the audio track and pauses the playback if necessary.
 *
 * @param pDevice A pointer to the audio device structure.
 * @param pOutput A pointer to the buffer where the decoded audio frames will be
 * written.
 * @param pInput A pointer to the buffer where the input audio frames would be
 * read from (unused in this function).
 * @param frameCount The number of audio frames to process.
 *
 * The function uses the ncurses library to draw a bar graph representing the
 * audio data. If playback is paused, the output buffer is filled with silence.
 */

void duckwave_playback_callback_with_visual(ma_device* pDevice, void* pOutput,
                                            const void* pInput,
                                            unsigned int frameCount)
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
    for (int i = 0; i < NUM_BARS; i++) {
      float sum = 0;
      for (int j = 0; j < samples_per_bar; j++) {
        sum += fabsf(audio_data[i * samples_per_bar + j]);
      }
      bar_data[i] = sum / samples_per_bar;
    }

    song_cursor += (float)frameCount / pDevice->sampleRate;
    draw_bars_and_time(bar_data, NUM_BARS, song_cursor, song_duration);

    if (song_cursor >= song_duration) {
      song_finished = true;
      ma_device_stop(pDevice);
    }
  } else {
    memset(pOutput, 0,
           frameCount * ma_get_bytes_per_frame(pDevice->playback.format,
                                               pDevice->playback.channels));
  }

  (void)pInput;
}

void cleanup()
{
  endwin();
  ma_device_uninit(&dw_sdata.device);
  fflush(stdout);
}

void init_ncurses()
{
  setlocale(LC_ALL, ""); // Configurar a localidade para UTF-8

  initscr();
  start_color();
  cbreak();
  noecho();
  curs_set(FALSE);
  timeout(0); // Permitir getch() não bloqueante

  // Inicializar pares de cores para o gradiente
  init_pair(1, COLOR_RED, COLOR_BLACK);
  init_pair(2, COLOR_YELLOW, COLOR_BLACK);
  init_pair(3, COLOR_GREEN, COLOR_BLACK);
  init_pair(4, COLOR_CYAN, COLOR_BLACK);
  init_pair(5, COLOR_BLUE, COLOR_BLACK);
  init_pair(6, COLOR_MAGENTA, COLOR_BLACK);

  atexit(cleanup);
}
