// playmp3.

#ifndef PLAYMP3_h
#define PLAYMP3_h



// DEBUG


#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include "audio_tools/miniaudio.h"

typedef struct {
  size_t buffer_size, done;
  int driver, err, channels, encoding;
  long rate;
  char *track;
  char *buffer;
} PlayMP3;

//typedef struct {
  //AVFormatContext *formatCtx;
  //AVCodecContext *codecCtx;
  //AVPacket *packet;
  //AVFrame *frame;
  //SwrContext *swrCtx;
  //uint8_t *buffer;
  //size_t buffer_size; // Adicionado
  //int streamIndex;
  //ao_device *dev;
  //ao_sample_format format;
  //int isPaused;
//} //PlayAudio;


typedef struct PlayAudio {
    AVFormatContext *formatCtx;
    AVCodecContext *codecCtx;
    AVPacket *packet;
    AVFrame *frame;
    SwrContext *swrCtx;
    uint8_t *buffer;
    int streamIndex;
    int isPaused;
    ma_decoder decoder;
    ma_device device;
} PlayAudio;

void init_PlayMP3(PlayMP3 *mp3);
void setMusic(PlayMP3 *mp3, char *track);
void play(PlayMP3 *mp3);
void cleanup_PlayMP3(PlayMP3 *mp3);

void init_PlayAudio(PlayAudio *audio);
void setMusicAudio(PlayAudio *audio, char *filename);
void playAudio(PlayAudio *audio);
void cleanup_PlayAudio(PlayAudio *audio);

#endif
