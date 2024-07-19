
#include <ncurses.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#include "../include/animation.h"
#include "../include/duckwave.h"

float song_duration = 0.0f, song_cursor = 0.0f;
bool song_finished = false;
bool is_paused = false;

DuckWaveSoundData dw_sdata;

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
  ma_result result = ma_decoder_get_length_in_pcm_frames(&dw_sdata.decoder,
                                                         &length_in_pcm_frames);
  if (result != MA_SUCCESS) 
  {
    printf("Failed to get length of audio file.\n");
    return -1;
  }
  song_duration =
      (float)length_in_pcm_frames / dw_sdata.decoder.outputSampleRate;

  dw_sdata.device_config.dataCallback = duckwave_playback_callback_with_visual;

  init_ncurses();

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

  while (1) {
    int timeout_ms = (int)((song_duration - song_cursor) * 1000);
    timeout(timeout_ms);

    int ch = getch();
    if (ch != ERR) {
      if (ch == ' ') {
        is_paused = !is_paused;
        if (is_paused) {
          ma_device_stop(&dw_sdata.device);
        } else {
          ma_device_start(&dw_sdata.device);
        }
      } else {
        break;
      }
    }

    if (song_finished) {
      break;
    }
  }
  

  cleanup();

  if (song_finished) 
  {
    printf("Playback finished. Exiting...\n");
  }

  return EXIT_SUCCESS;
}