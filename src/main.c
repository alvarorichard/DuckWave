#include <math.h>
#include <ncurses.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "../include/duckwave.h"

// int main(int argc, char *argv[]) {
//     if (argc < 2) {
//         printf("You need to specify a song.\n");
//         return EXIT_FAILURE;
//     } else if (argc > 2) {
//         printf("Multiple song files are not allowed right now.\n");
//         return EXIT_FAILURE;
//     }

//     char* song_file = argv[1];
//     DuckWaveSoundData dw_sdata;

//     duckwave_init_file_decoder(&dw_sdata, song_file);
//     duckwave_init_device_playback(&dw_sdata);
//     duckwave_start_playsound_thread(&dw_sdata);

//     if (!ma_device_is_started(&dw_sdata.device)) {
//         printf("Could not start the song.\n");
//         return EXIT_FAILURE;
//     }

//     float song_duration = 0.0f, song_cursor = 0.0f; // Inicialização das
//     variáveis char duration_timestamp[12], cursor_timestamp[12];

//     ma_data_source_get_length_in_seconds(&dw_sdata.decoder, &song_duration);
//     generate_timestamp(song_duration, duration_timestamp);

//     printf("\x1b[2J\x1b[?25h\x1b[?25l");  // clear the screen & hide cursor

//     while (song_cursor < song_duration) {
//         ma_data_source_get_cursor_in_seconds(&dw_sdata.decoder,
//         &song_cursor); generate_timestamp(song_cursor, cursor_timestamp);

//         printf("\x1b[H");  // move the write cursor to the top left, to
//         overwrite the timestamp

//         printf("%s/%s\n", cursor_timestamp, duration_timestamp);
//     }

//     ma_device_uninit(&dw_sdata.device);
//     ma_decoder_uninit(&dw_sdata.decoder);

//     return EXIT_SUCCESS;
// }

// Função para desenhar gráficos de barras
void draw_bars(float *data, int count) {
  clear();
  int max_height = LINES - 1;
  for (int i = 0; i < count; i++) {
    int height = (int)(data[i] * max_height);
    for (int j = 0; j < height; j++) {
      mvprintw(max_height - j, i * 2, "|");
    }
  }
  refresh();
}

// Callback modificado para capturar dados de áudio
void duckwave_playback_callback_with_visual(ma_device *pDevice, void *pOutput,
                                            const void *pInput,
                                            unsigned int frameCount) {
  ma_decoder *pDecoder = pDevice->pUserData;
  if (pDecoder == NULL)
    return;

  ma_decoder_read_pcm_frames(pDecoder, pOutput, frameCount, NULL);

  // Processar dados de áudio para visualização
  float *audio_data = (float *)pOutput;
  float bar_data[80];
  int samples_per_bar = frameCount / 80;
  for (int i = 0; i < 80; i++) {
    float sum = 0;
    for (int j = 0; j < samples_per_bar; j++) {
      sum += fabs(audio_data[i * samples_per_bar + j]);
    }
    bar_data[i] = sum / samples_per_bar;
  }
  draw_bars(bar_data, 80);

  (void)pInput;
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Usage: %s <audio_file>\n", argv[0]);
    return -1;
  }

  DuckWaveSoundData dw_sdata;
  duckwave_init_file_decoder(&dw_sdata, argv[1]);
  duckwave_init_device_playback(&dw_sdata);

  // Substituir o callback padrão pelo callback com visualização
  dw_sdata.device_config.dataCallback = duckwave_playback_callback_with_visual;

  initscr();
  cbreak();
  noecho();
  curs_set(FALSE);

  ma_result result =
      ma_device_init(NULL, &dw_sdata.device_config, &dw_sdata.device);
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
