#pragma once

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
#include <stddef.h>

typedef struct {
  AVFormatContext *formatCtx;
  AVCodecContext *codecCtx;
  AVPacket *packet;
  AVFrame *frame;
  SwrContext *swrCtx;
  uint8_t *buffer;
  size_t buffer_size;
  int streamIndex;
  int isPaused;
} PlayAudio;

void init_PlayAudio(PlayAudio *audio);
void setMusicAudio(PlayAudio *audio, char *filename);
void playAudio(PlayAudio *audio);
void cleanup_PlayAudio(PlayAudio *audio);
