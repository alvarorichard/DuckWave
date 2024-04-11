#include <stdbool.h>
#include <stdio.h>

#include "../include/duckwave.h"

int main(int argc, char *argv[]) {
	if (argc < 2) {
		printf("You need to specify a song.\n");

		return 1;

	} else if (argc > 2) {
		printf("Multiple song files are not allowed right now.\n");

		return 2;
	}

	char* song_file = argv[1];

	DuckWaveSoundData dw_sdata;

	duckwave_init_file_decoder(&dw_sdata, song_file);
	duckwave_init_device_playback(&dw_sdata);
	duckwave_start_playsound_thread(&dw_sdata);

	///  NOTE: O código abixo é só pra testar o comportamento da aplicação.

	if (!ma_device_is_started(&dw_sdata.device)) {
		printf("Could not start the song.\n");

		return 3;
	}

	float song_duration, song_cursor;
	char duration_timestamp[12], cursor_timestamp[12];

	ma_data_source_get_length_in_seconds(&dw_sdata.decoder, &song_duration);
	generate_timestamp(song_duration, duration_timestamp);

	printf("\x1b[2J\x1b[?25h\x1b[?25l");  // clear the screen & hide cursor

	while (song_cursor < song_duration) {
		ma_data_source_get_cursor_in_seconds(&dw_sdata.decoder, &song_cursor);
		generate_timestamp(song_cursor, cursor_timestamp);

		printf("\x1b[H");  // move the write cursor to the top left, to overwrite the timestamp

		printf("%s/%s\n", cursor_timestamp, duration_timestamp);
	}

	ma_device_uninit(&dw_sdata.device);
	ma_decoder_uninit(&dw_sdata.decoder);

	return 0;
}
