#pragma once

#include "./miniaudio.h"

///  NOTE: Embaixo dessa função, deve haver outras similares; a documentação será a mesma.
///  WARN: Todas as funções com `callback` no nome serão executadas em uma thread separada.

/**
 * Essas funções são responsáveis por ler os dados do objeto `decoder` pra
 * conseguir tocar a música de fato. Mas **atenção**, essas funções serão
 * executadas numa thread separada, cuidado na hora de programar!
 */
void duckwave_default_playback_callback(ma_device*, void*, const void*, unsigned int);

/**
 * Struct com os objetos que o miniaudio precisa pra configurar um device novo
 * pra começar a tocar a música em questão.
 */
typedef struct DuckWaveSoundData {
	ma_decoder decoder;
	ma_device_config device_config;
	ma_device device;
} DuckWaveSoundData;

/**
 * Inicia o decoder usando uma string com o caminho para o arquivo, talvez não
 * com alguns formatos como o MP4.
 */
void duckwave_init_file_decoder(DuckWaveSoundData*, char*);

/**
 * Uma vez que o decoder já foi configurado, essa função vai simplesmente
 * construir o device pra tocar a música. Usando o tipo playback do miniaudio.
 */
void duckwave_init_device_playback(DuckWaveSoundData*);


/**
 * Abstração para a biblioteca `miniaudio.h`, ela vai iniciar a rotina pra
 * tocar a música em background (numa thread separada); verificando e tratando
 * os erros a medida que eles vão aparecendo.
 */
void duckwave_start_playsound_thread(DuckWaveSoundData*);
