// playmp3.c
#include "playmp3.h"
#include <fcntl.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"




// Implementações das funções PlayMP3...

int isSpacePressed() {
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch == ' ')
        return 1;
    return 0;
}

void data_callback(ma_device *pDevice, void *pOutput, const void *pInput, ma_uint32 frameCount) {
    ma_decoder *pDecoder = (ma_decoder *)pDevice->pUserData;
    if (pDecoder == NULL) {
        return;
    }

    ma_decoder_read_pcm_frames(pDecoder, pOutput, frameCount, NULL);
    (void)pInput;
}

void init_PlayAudio(PlayAudio *audio) {
    audio->formatCtx = NULL;
    audio->codecCtx = NULL;
    audio->packet = av_packet_alloc();
    audio->frame = av_frame_alloc();
    audio->swrCtx = NULL;
    audio->buffer = NULL;
    audio->streamIndex = -1;
    audio->isPaused = 0;
}

void setMusicAudio(PlayAudio *audio, char *filename) {
    if (avformat_open_input(&audio->formatCtx, filename, NULL, NULL) != 0) {
        fprintf(stderr, "Could not open input file '%s'\n", filename);
        exit(1);
    }

    if (avformat_find_stream_info(audio->formatCtx, NULL) < 0) {
        fprintf(stderr, "Could not find stream information\n");
        exit(1);
    }

    audio->streamIndex = -1;
    for (int i = 0; i < audio->formatCtx->nb_streams; i++) {
        if (audio->formatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            audio->streamIndex = i;
            break;
        }
    }

    if (audio->streamIndex == -1) {
        fprintf(stderr, "Could not find audio stream\n");
        exit(1);
    }

    AVCodecParameters *codecpar = audio->formatCtx->streams[audio->streamIndex]->codecpar;
    const AVCodec *codec = avcodec_find_decoder(codecpar->codec_id);

    if (!codec) {
        fprintf(stderr, "Could not find codec\n");
        exit(1);
    }

    audio->codecCtx = avcodec_alloc_context3(codec);
    if (!audio->codecCtx) {
        fprintf(stderr, "Could not allocate audio codec context\n");
        exit(1);
    }

    if (avcodec_parameters_to_context(audio->codecCtx, codecpar) < 0) {
        fprintf(stderr, "Could not copy codec parameters to codec context\n");
        exit(1);
    }

    if (avcodec_open2(audio->codecCtx, codec, NULL) < 0) {
        fprintf(stderr, "Could not open codec\n");
        exit(1);
    }

    ma_result result;
    ma_decoder_config decoderConfig = ma_decoder_config_init(ma_format_f32, 2, 44100);
    result = ma_decoder_init_file(filename, &decoderConfig, &audio->decoder);
    if (result != MA_SUCCESS) {
        fprintf(stderr, "Could not initialize audio decoder\n");
        exit(1);
    }

    ma_device_config deviceConfig = ma_device_config_init(ma_device_type_playback);
    deviceConfig.playback.format = audio->decoder.outputFormat;
    deviceConfig.playback.channels = audio->decoder.outputChannels;
    deviceConfig.sampleRate = audio->decoder.outputSampleRate;
    deviceConfig.dataCallback = data_callback;
    deviceConfig.pUserData = &audio->decoder;

    result = ma_device_init(NULL, &deviceConfig, &audio->device);
    if (result != MA_SUCCESS) {
        fprintf(stderr, "Could not initialize audio playback device\n");
        ma_decoder_uninit(&audio->decoder);
        exit(1);
    }
}

void playAudio(PlayAudio *audio) {
    ma_result result = ma_device_start(&audio->device);
    if (result != MA_SUCCESS) {
        fprintf(stderr, "Failed to start audio playback device\n");
        ma_decoder_uninit(&audio->decoder);
        exit(1);
    }

    while (ma_device_is_started(&audio->device)) {
        if (isSpacePressed()) {
            audio->isPaused = !audio->isPaused;
            if (audio->isPaused) {
                ma_device_stop(&audio->device);
            } else {
                ma_device_start(&audio->device);
            }
            usleep(300000); // 300ms
        }
        usleep(10000); // 10ms
    }

    ma_device_uninit(&audio->device);
    ma_decoder_uninit(&audio->decoder);
}

void cleanup_PlayAudio(PlayAudio *audio) {
    if (audio->codecCtx) {
        avcodec_free_context(&audio->codecCtx);
    }
    if (audio->formatCtx) {
        avformat_close_input(&audio->formatCtx);
    }
    if (audio->packet) {
        av_packet_free(&audio->packet);
    }
    if (audio->frame) {
        av_frame_free(&audio->frame);
    }
}