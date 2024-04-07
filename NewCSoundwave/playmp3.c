#include "playmp3.h"
#include <SDL2/SDL_audio.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
#include <libavutil/opt.h>
#include <libavutil/channel_layout.h>
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>  
#include <signal.h>
#include <ncurses.h>


// Implementações das funções PlayMP3...
volatile sig_atomic_t keep_running = 1;

void sigint_handler(int sig) {
    (void)sig;  // Para evitar avisos sobre o parâmetro não utilizado
    printf("SIGINT recebido, encerrando a reprodução...\n");
    keep_running = 0;
}



// int isSpacePressed() {
//   struct termios oldt, newt;
//   int ch;
//   int oldf;

//   // Salva as configurações atuais do terminal
//   tcgetattr(STDIN_FILENO, &oldt);
//   newt = oldt;
//   // Desativa o modo canônico (input line-by-line) e eco
//   newt.c_lflag &= ~(ICANON | ECHO);
//   // Aplica as novas configurações ao terminal
//   tcsetattr(STDIN_FILENO, TCSANOW, &newt);
//   // Salva o estado atual das flags do arquivo e ativa o modo não bloqueante
//   oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
//   fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

//   // Lê um caractere do stdin sem bloquear
//   ch = getchar();

//   // Restaura as configurações originais do terminal
//   tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
//   fcntl(STDIN_FILENO, F_SETFL, oldf);

//   // Verifica se a tecla pressionada foi a tecla de espaço
//   if (ch == ' ')
//     return 1;
//   return 0;
// }

int isSpacePressed() {
    int ch = getch();
    if (ch == ' ') {
        return 1;
    }
    return 0;
}



void init_PlayAudio(PlayAudio *audio) {
  // Inicializa o FFmpeg
  avformat_network_init();
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
    // Abre o arquivo de áudio e encontra o fluxo de áudio
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
        fprintf(stderr, "Could not find codec for %s\n", filename);
        exit(1);
    }

    // Aloca o contexto do codec e copia os parâmetros para ele
    audio->codecCtx = avcodec_alloc_context3(codec);
    if (!audio->codecCtx) {
        fprintf(stderr, "Could not allocate audio codec context\n");
        exit(1);
    }

    if (avcodec_parameters_to_context(audio->codecCtx, codecpar) < 0) {
        fprintf(stderr, "Could not copy codec parameters to codec context\n");
        exit(1);
    }

    // Abre o codec
    if (avcodec_open2(audio->codecCtx, codec, NULL) < 0) {
        fprintf(stderr, "Could not open codec\n");
        exit(1);
    }

    // Configura o SwrContext para a conversão de formato de áudio
    audio->swrCtx = swr_alloc();
    if (!audio->swrCtx) {
        fprintf(stderr, "Could not allocate resampler context\n");
        exit(1);
    }
    av_opt_set_int(audio->swrCtx, "in_channel_layout", codecpar->channel_layout, 0);
    av_opt_set_int(audio->swrCtx, "in_sample_rate", codecpar->sample_rate, 0);
    av_opt_set_sample_fmt(audio->swrCtx, "in_sample_fmt", codecpar->format, 0);
    av_opt_set_int(audio->swrCtx, "out_channel_layout", AV_CH_LAYOUT_STEREO, 0);
    av_opt_set_int(audio->swrCtx, "out_sample_rate", 48000, 0);  // Exemplo de taxa de amostragem de saída
    av_opt_set_sample_fmt(audio->swrCtx, "out_sample_fmt", AV_SAMPLE_FMT_S16, 0);

    if (swr_init(audio->swrCtx) < 0) {
        fprintf(stderr, "Could not initialize resampler context\n");
        exit(1);
    }
}


void decodeAudio(PlayAudio *audio) {
    int channels = av_get_channel_layout_nb_channels(audio->codecCtx->channel_layout);
  int bytes_per_sample = av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);
  int dst_bufsize = av_samples_get_buffer_size(NULL, channels, audio->frame->nb_samples, AV_SAMPLE_FMT_S16, 1);
  audio->buffer = (uint8_t *)av_malloc(dst_bufsize);


  if (av_read_frame(audio->formatCtx, audio->packet) < 0) {
    fprintf(stderr, "Could not read frame\n");
    exit(1);
  }

  while (av_read_frame(audio->formatCtx, audio->packet) >= 0) {
    if (audio->packet->stream_index == audio->streamIndex) {
      if (avcodec_send_packet(audio->codecCtx, audio->packet) < 0) {
        fprintf(stderr, "Could not send packet for decoding\n");
        break;
      }

      while (avcodec_receive_frame(audio->codecCtx, audio->frame) == 0) {
        swr_convert(audio->swrCtx, &audio->buffer, audio->frame->nb_samples, (const uint8_t **)audio->frame->data, audio->frame->nb_samples);
      }
    }
    av_packet_unref(audio->packet);
  }

  if (audio->packet->stream_index == audio->streamIndex) {
    if (avcodec_send_packet(audio->codecCtx, audio->packet) < 0) {
      fprintf(stderr, "Could not send packet for decoding\n");
      exit(1);
    }

    while (avcodec_receive_frame(audio->codecCtx, audio->frame) == 0) {
      int nb_samples = audio->frame->nb_samples;
      int channels = audio->codecCtx->channels;
      int bytes_per_sample = av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);
      int dst_bufsize = av_samples_get_buffer_size(NULL, channels, nb_samples, AV_SAMPLE_FMT_S16, 1);
      audio->buffer = (uint8_t *)av_malloc(dst_bufsize);

      swr_convert(audio->swrCtx, &audio->buffer, nb_samples, (const uint8_t **)audio->frame->data, nb_samples);

      // Reproduz o áudio decodificado
      playAudio(audio);

      av_free(audio->buffer);
    }
  }

  av_packet_unref(audio->packet);
}



// void playAudio(PlayAudio *audio) {
//     // Inicializa o subsistema de áudio SDL
//     if (SDL_Init(SDL_INIT_AUDIO) < 0) {
//         fprintf(stderr, "Não foi possível inicializar o SDL: %s\n", SDL_GetError());
//         exit(1);
//     }

//     // Define as especificações de áudio desejadas
//     SDL_AudioSpec want, have;
//     SDL_AudioDeviceID dev;
//     SDL_memset(&want, 0, sizeof(want));
//     want.freq = audio->codecCtx->sample_rate;
//     want.format = AUDIO_S16SYS;
//     want.channels = av_get_channel_layout_nb_channels(audio->codecCtx->channel_layout);
//     want.samples = 4096;
//     want.callback = NULL;  // Usaremos SDL_QueueAudio() em vez de um callback

//     // Abre o dispositivo de áudio
//     dev = SDL_OpenAudioDevice(NULL, 0, &want, &have, 0);
//     if (dev == 0) {
//         fprintf(stderr, "Falha ao abrir o dispositivo de áudio: %s\n", SDL_GetError());
//         SDL_Quit();
//         exit(1);
//     }

//     // Inicia a reprodução de áudio
//     SDL_PauseAudioDevice(dev, 0);

//     // Calcula o tamanho do buffer de saída necessário
//     int out_samples = av_rescale_rnd(swr_get_delay(audio->swrCtx, audio->codecCtx->sample_rate) + audio->frame->nb_samples, audio->codecCtx->sample_rate, audio->codecCtx->sample_rate, AV_ROUND_UP);
//     int out_buffer_size = av_samples_get_buffer_size(NULL, want.channels, out_samples, AV_SAMPLE_FMT_S16, 1);
//     uint8_t *out_buffer = (uint8_t *)malloc(out_buffer_size);  // Usando malloc

//     // Processa o áudio decodificado
//     int ret;
//     while ((ret = av_read_frame(audio->formatCtx, audio->packet)) >= 0) {
//         if (audio->packet->stream_index == audio->streamIndex) {
//             if (avcodec_send_packet(audio->codecCtx, audio->packet) < 0) {
//                 fprintf(stderr, "Erro ao enviar um pacote para decodificação\n");
//                 break;
//             }

//             while (avcodec_receive_frame(audio->codecCtx, audio->frame) >= 0) {
//                 // Converte o quadro para o formato desejado
//                 int len = swr_convert(audio->swrCtx, &out_buffer, out_samples, (const uint8_t **)audio->frame->data, audio->frame->nb_samples);
//                 if (len < 0) {
//                     fprintf(stderr, "Erro ao converter o quadro de áudio\n");
//                     break;
//                 }

//                 // Envia o áudio decodificado para o dispositivo de áudio SDL
//                 SDL_QueueAudio(dev, out_buffer, len * want.channels * av_get_bytes_per_sample(AV_SAMPLE_FMT_S16));
//             }
//         }
//         av_packet_unref(audio->packet);
//     }

//     // Aguarda até que todo o áudio seja reproduzido
//     while (SDL_GetQueuedAudioSize(dev) > 0) {
//         SDL_Delay(100);
//     }

//     // Limpa e fecha o dispositivo de áudio
//     SDL_CloseAudioDevice(dev);
//     SDL_Quit();

//     // Libera o buffer de saída
//     free(out_buffer);  // Usando free
// }


// void playAudio(PlayAudio *audio) {
//     // Inicializa o subsistema de áudio SDL
//     if (SDL_Init(SDL_INIT_AUDIO) < 0) {
//         fprintf(stderr, "Não foi possível inicializar o SDL: %s\n", SDL_GetError());
//         exit(1);
//     }

//     // Define as especificações de áudio desejadas
//     SDL_AudioSpec want, have;
//     SDL_AudioDeviceID dev;
//     SDL_memset(&want, 0, sizeof(want));
//     want.freq = audio->codecCtx->sample_rate;
//     want.format = AUDIO_S16SYS;
//     want.channels = av_get_channel_layout_nb_channels(audio->codecCtx->channel_layout);
//     want.samples = 4096;
//     want.callback = NULL;  // Usaremos SDL_QueueAudio() em vez de um callback

//     // Abre o dispositivo de áudio
//     dev = SDL_OpenAudioDevice(NULL, 0, &want, &have, 0);
//     if (dev == 0) {
//         fprintf(stderr, "Falha ao abrir o dispositivo de áudio: %s\n", SDL_GetError());
//         SDL_Quit();
//         exit(1);
//     }

//     // Inicia a reprodução de áudio
//     SDL_PauseAudioDevice(dev, 0);

//     // Processa o áudio decodificado
//     int ret;
//     while (keep_running && (ret = av_read_frame(audio->formatCtx, audio->packet)) >= 0) {
//         if (isSpacePressed()) {
//             audio->isPaused = !audio->isPaused;
//             SDL_PauseAudioDevice(dev, audio->isPaused);
//         }

//         if (audio->isPaused || !keep_running) {
//             continue;  // Pula para a próxima iteração do loop se a reprodução estiver pausada ou se o sinal SIGINT foi recebido
//         }

//         if (audio->packet->stream_index == audio->streamIndex) {
//             if (avcodec_send_packet(audio->codecCtx, audio->packet) < 0) {
//                 fprintf(stderr, "Erro ao enviar um pacote para decodificação\n");
//                 break;
//             }

//             while (avcodec_receive_frame(audio->codecCtx, audio->frame) >= 0) {
//                 // Calcula o tamanho do buffer de saída necessário
//                 int out_samples = av_rescale_rnd(swr_get_delay(audio->swrCtx, audio->codecCtx->sample_rate) + audio->frame->nb_samples, audio->codecCtx->sample_rate, audio->codecCtx->sample_rate, AV_ROUND_UP);
//                 if (out_samples <= 0) {
//                     continue;  // Pula para a próxima iteração do loop se não houver amostras para processar
//                 }

//                 int out_buffer_size = av_samples_get_buffer_size(NULL, want.channels, out_samples, AV_SAMPLE_FMT_S16, 1);
//                 if (out_buffer_size < 0) {
//                     fprintf(stderr, "Erro ao calcular o tamanho do buffer de saída: %d\n", out_buffer_size);
//                     SDL_CloseAudioDevice(dev);
//                     SDL_Quit();
//                     exit(1);
//                 }

//                 uint8_t *out_buffer = (uint8_t *)malloc(out_buffer_size);
//                 if (!out_buffer) {
//                     fprintf(stderr, "Erro ao alocar o buffer de saída\n");
//                     SDL_CloseAudioDevice(dev);
//                     SDL_Quit();
//                     exit(1);
//                 }

//                 // Converte o quadro para o formato desejado
//                 int len = swr_convert(audio->swrCtx, &out_buffer, out_samples, (const uint8_t **)audio->frame->data, audio->frame->nb_samples);
//                 if (len < 0) {
//                     fprintf(stderr, "Erro ao converter o quadro de áudio\n");
//                     free(out_buffer);
//                     continue;  // Pula para a próxima iteração do loop se houver erro na conversão
//                 }

//                 // Envia o áudio decodificado para o dispositivo de áudio SDL
//                 SDL_QueueAudio(dev, out_buffer, len * want.channels * av_get_bytes_per_sample(AV_SAMPLE_FMT_S16));

//                 // Libera o buffer de saída
//                 free(out_buffer);
//             }
//         }
//         av_packet_unref(audio->packet);
//     }

//     // Aguarda até que todo o áudio seja reproduzido ou até que a reprodução seja interrompida
//     while (keep_running && SDL_GetQueuedAudioSize(dev) > 0) {
//         SDL_Delay(100);
//     }

//     // Limpa e fecha o dispositivo de áudio
//     SDL_CloseAudioDevice(dev);
//     SDL_Quit();
// }


void playAudio(PlayAudio *audio) {
    // Inicializa o subsistema de áudio SDL
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "Não foi possível inicializar o SDL: %s\n", SDL_GetError());
        exit(1);
    }

    // Define as especificações de áudio desejadas
    SDL_AudioSpec want, have;
    SDL_AudioDeviceID dev;
    SDL_memset(&want, 0, sizeof(want));
    want.freq = audio->codecCtx->sample_rate;
    want.format = AUDIO_S16SYS;
    want.channels = av_get_channel_layout_nb_channels(audio->codecCtx->channel_layout);
    want.samples = 4096;
    want.callback = NULL;  // Usaremos SDL_QueueAudio() em vez de um callback

    // Abre o dispositivo de áudio
    dev = SDL_OpenAudioDevice(NULL, 0, &want, &have, 0);
    if (dev == 0) {
        fprintf(stderr, "Falha ao abrir o dispositivo de áudio: %s\n", SDL_GetError());
        SDL_Quit();
        exit(1);
    }

    // Inicia a reprodução de áudio
    SDL_PauseAudioDevice(dev, 0);

    SDL_Event event;
    int ret;
    while (keep_running && (ret = av_read_frame(audio->formatCtx, audio->packet)) >= 0) {
        // Verifica se há eventos pendentes
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_SPACE) {
                    audio->isPaused = !audio->isPaused;
                    SDL_PauseAudioDevice(dev, audio->isPaused);
                }
            }
        }

        if (audio->isPaused || !keep_running) {
            continue;  // Pula para a próxima iteração do loop se a reprodução estiver pausada ou se o sinal SIGINT foi recebido
        }

        if (audio->packet->stream_index == audio->streamIndex) {
            if (avcodec_send_packet(audio->codecCtx, audio->packet) < 0) {
                fprintf(stderr, "Erro ao enviar um pacote para decodificação\n");
                break;
            }

            while (avcodec_receive_frame(audio->codecCtx, audio->frame) >= 0) {
                // Calcula o tamanho do buffer de saída necessário
                int out_samples = av_rescale_rnd(swr_get_delay(audio->swrCtx, audio->codecCtx->sample_rate) + audio->frame->nb_samples, audio->codecCtx->sample_rate, audio->codecCtx->sample_rate, AV_ROUND_UP);
                if (out_samples <= 0) {
                    continue;  // Pula para a próxima iteração do loop se não houver amostras para processar
                }

                int out_buffer_size = av_samples_get_buffer_size(NULL, want.channels, out_samples, AV_SAMPLE_FMT_S16, 1);
                if (out_buffer_size < 0) {
                    fprintf(stderr, "Erro ao calcular o tamanho do buffer de saída: %d\n", out_buffer_size);
                    SDL_CloseAudioDevice(dev);
                    SDL_Quit();
                    exit(1);
                }

                uint8_t *out_buffer = (uint8_t *)malloc(out_buffer_size);
                if (!out_buffer) {
                    fprintf(stderr, "Erro ao alocar o buffer de saída\n");
                    SDL_CloseAudioDevice(dev);
                    SDL_Quit();
                    exit(1);
                }

                // Converte o quadro para o formato desejado
                int len = swr_convert(audio->swrCtx, &out_buffer, out_samples, (const uint8_t **)audio->frame->data, audio->frame->nb_samples);
                if (len < 0) {
                    fprintf(stderr, "Erro ao converter o quadro de áudio\n");
                    free(out_buffer);
                    continue;  // Pula para a próxima iteração do loop se houver erro na conversão
                }

                // Envia o áudio decodificado para o dispositivo de áudio SDL
                SDL_QueueAudio(dev, out_buffer, len * want.channels * av_get_bytes_per_sample(AV_SAMPLE_FMT_S16));

                // Libera o buffer de saída
                free(out_buffer);
            }
        }
        av_packet_unref(audio->packet);
    }

    // Aguarda até que todo o áudio seja reproduzido ou até que a reprodução seja interrompida
    while (keep_running && SDL_GetQueuedAudioSize(dev) > 0) {
        SDL_Delay(100);
    }

    // Limpa e fecha o dispositivo de áudio
    SDL_CloseAudioDevice(dev);
    SDL_Quit();
}



void cleanup_PlayAudio(PlayAudio *audio) {
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
        av_free(audio->buffer);
        audio->buffer = NULL;
    }
    avformat_network_deinit();
}

// Fim das implementações das funções PlayMP3...



