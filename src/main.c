#include <stdbool.h>
#include <stdio.h>

#include "duckwave.h"

int main(int argc, char *argv[]) {
	DuckWaveSoundData dw_sdata;

	duckwave_init_file_decoder(&dw_sdata, "music.flac");
	duckwave_init_device_playback(&dw_sdata);
	duckwave_start_playsound_thread(&dw_sdata);

	printf("The music should be playing by now.\n");

	while (true) {}

	return 0;
}
