#include <stdio.h>
#include <stdlib.h>

#define MINIAUDIO_IMPLEMENTATION

#include "duckwave.h"

void generate_timestamp(float secs, char* res) {
	int hours = (int)secs / 3600;

	secs = (int)secs % 3600;

	int mins = (int)secs / 60;

	secs = (int)secs % 60;

	if (hours > 0)
		sprintf(res, "%d:%02d:%02.0f", hours, mins, secs);

	else
		sprintf(res, "%02d:%02.0f", mins, secs);
}

void duckwave_default_playback_callback(ma_device* pDevice, void* pOutput, const void* _pInput, unsigned int frameCount) {
	ma_decoder* pDecoder = pDevice->pUserData;

	if (pDecoder == NULL)
		return;

	ma_decoder_read_pcm_frames(pDecoder, pOutput, frameCount, NULL);

	(void)_pInput;
}

void duckwave_init_file_decoder(DuckWaveSoundData* dw_sdata, char* file) {
	ma_result result;

	result = ma_decoder_init_file(file, NULL, &dw_sdata->decoder);

	if (result != MA_SUCCESS) {
		printf("\nCould not decode current %s file.\n", file);
		printf("%s.\n\n", ma_result_description(result));

		exit(1);
	}
}

void duckwave_init_device_playback(DuckWaveSoundData* dw_sdata) {
	ma_decoder* decoder = &dw_sdata->decoder;
	ma_device_config* device_config = &dw_sdata->device_config;

	*device_config = ma_device_config_init(ma_device_type_playback);

	device_config->playback.format = decoder->outputFormat;
	device_config->playback.channels = decoder->outputChannels;
	device_config->sampleRate = decoder->outputSampleRate;
	device_config->dataCallback = duckwave_default_playback_callback;
	device_config->pUserData = decoder;
}

void duckwave_start_playsound_thread(DuckWaveSoundData* dw_sdata) {
	ma_result result;

	ma_decoder* decoder = &dw_sdata->decoder;
	ma_device* device = &dw_sdata->device;
	ma_device_config* device_config = &dw_sdata->device_config;

	result = ma_device_init(NULL, device_config, device);

	if (result != MA_SUCCESS) {
		printf("\nCannot initilize device.\n");
		printf("%s.\n\n", ma_result_description(result));

		ma_decoder_uninit(decoder);

		exit(2);
	}

	result = ma_device_start(device);

	if (result != MA_SUCCESS) {
		printf("\nCannot start device.\n");
		printf("%s.\n\n", ma_result_description(result));

		ma_device_uninit(device);
		ma_decoder_uninit(decoder);

		exit(3);
	}
}


