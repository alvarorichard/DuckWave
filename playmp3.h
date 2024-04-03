// playmp3.
#pragma ONCE

// DEBUG

#include <ao/ao.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
#include <mpg123.h>
#include <stddef.h>

typedef struct {
  size_t buffer_size, done;
  int driver, err, channels, encoding;
  long rate;
  char *track;
  mpg123_handle *mh;
  char *buffer;
  ao_sample_format format;
  ao_device *dev;
} PlayMP3;

typedef struct {
  AVFormatContext *formatCtx;
  AVCodecContext *codecCtx;
  AVPacket *packet;
  AVFrame *frame;
  SwrContext *swrCtx;
  uint8_t *buffer;
  size_t buffer_size; // Adicionado
  int streamIndex;
  ao_device *dev;
  ao_sample_format format;
  int isPaused;
} PlayAudio;

void init_PlayMP3(PlayMP3 *mp3);
void setMusic(PlayMP3 *mp3, char *track);
void play(PlayMP3 *mp3);
void cleanup_PlayMP3(PlayMP3 *mp3);

void init_PlayAudio(PlayAudio *audio);
void setMusicAudio(PlayAudio *audio, char *filename);
void playAudio(PlayAudio *audio);
void cleanup_PlayAudio(PlayAudio *audio);
