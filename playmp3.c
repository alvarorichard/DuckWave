// playmp3.c

#include "playmp3.h"
#include <ao/ao.h>
#include <fcntl.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
#include <mpg123.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

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

void init_PlayAudio(PlayAudio *audio) {
  // av_register_all(); // Removido
  ao_initialize();
  audio->formatCtx = NULL;
  audio->codecCtx = NULL;
  audio->packet = av_packet_alloc();
  audio->frame = av_frame_alloc();
  audio->swrCtx = NULL;
  audio->buffer = NULL;
  audio->streamIndex = -1;
  audio->dev = NULL;
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
    if (audio->formatCtx->streams[i]->codecpar->codec_type ==
        AVMEDIA_TYPE_AUDIO) {
      audio->streamIndex = i;
      break;
    }
  }

  if (audio->streamIndex == -1) {
    fprintf(stderr, "Could not find audio stream\n");
    exit(1);
  }

  AVCodecParameters *codecpar =
      audio->formatCtx->streams[audio->streamIndex]->codecpar;
  // AVCodec *codec = avcodec_find_decoder(codecpar->codec_id);
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

  audio->format.bits = 16;
  audio->format.rate = audio->codecCtx->sample_rate;
  // audio->format.channels = audio->codecCtx->channels;
  // audio->format.channels = codecpar->channels;
  audio->format.channels = codecpar->channels;

  audio->format.byte_format = AO_FMT_NATIVE;
  audio->format.matrix = 0;
  audio->dev = ao_open_live(ao_default_driver_id(), &audio->format, NULL);
  if (!audio->dev) {
    fprintf(stderr, "Could not open audio device\n");
    exit(1);
  }

  audio->swrCtx = swr_alloc_set_opts(
      NULL, av_get_default_channel_layout(audio->format.channels),
      AV_SAMPLE_FMT_S16, audio->format.rate,
      av_get_default_channel_layout(codecpar->channels), codecpar->format,
      codecpar->sample_rate, 0, NULL);

  if (!audio->swrCtx) {
    fprintf(stderr, "Could not allocate resampler context\n");
    exit(1);
  }

  if (swr_init(audio->swrCtx) < 0) {
    fprintf(stderr, "Could not initialize resampler context\n");
    exit(1);
  }
}

void playAudio(PlayAudio *audio) {
  int ret;
  // Estimativa do número máximo de amostras de saída por quadro
  int max_out_samples_per_frame =
      audio->format
          .rate; // Supondo 1 segundo de áudio por quadro como caso extremo
  // Calcula o tamanho do buffer de saída necessário
  int out_size = av_samples_get_buffer_size(NULL, audio->format.channels,
                                            max_out_samples_per_frame,
                                            AV_SAMPLE_FMT_S16, 0);
  // Aloca o buffer de saída
  audio->buffer = (uint8_t *)malloc(out_size);
  if (!audio->buffer) {
    fprintf(stderr, "Could not allocate output buffer\n");
    exit(1);
  }

  while (av_read_frame(audio->formatCtx, audio->packet) >= 0) {
    if (audio->packet->stream_index == audio->streamIndex) {
      ret = avcodec_send_packet(audio->codecCtx, audio->packet);
      if (ret < 0) {
        fprintf(stderr, "Error sending a packet for decoding\n");
        break;
      }

      while (ret >= 0) {
        ret = avcodec_receive_frame(audio->codecCtx, audio->frame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
          break;
        } else if (ret < 0) {
          fprintf(stderr, "Error during decoding\n");
          break;
        }

        if (isSpacePressed()) {
          audio->isPaused = !audio->isPaused;
          usleep(300000); // 300ms
        }

        if (!audio->isPaused) {
          uint8_t *out[2] = {audio->buffer, NULL};
          int len = swr_convert(audio->swrCtx, out, max_out_samples_per_frame,
                                (const uint8_t **)audio->frame->data,
                                audio->frame->nb_samples);
          if (len < 0) {
            fprintf(stderr, "Swr convert error\n");
            break;
          }

          ao_play(audio->dev, (char *)audio->buffer,
                  len * audio->format.channels *
                      av_get_bytes_per_sample(AV_SAMPLE_FMT_S16));
        } else {
          usleep(100000); // 100ms
        }
      }
    }
    av_packet_unref(audio->packet);
  }

  // Libera o buffer de saída após o término da reprodução
  free(audio->buffer);
}

void cleanup_PlayAudio(PlayAudio *audio) {
  if (audio->dev) {
    ao_close(audio->dev);
  }
  if (audio->swrCtx) {
    swr_free(&audio->swrCtx);
  }
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
  if (audio->buffer) {
    free(audio->buffer);
  }
  ao_shutdown();
}
