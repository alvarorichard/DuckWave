#ifndef ANIMATION_H
#define ANIMATION_H

#include "../include/miniaudio.h"
#include "../include/duckwave.h"

extern float song_duration;
extern float song_cursor;
extern bool song_finished;
extern bool is_paused;
extern DuckWaveSoundData dw_sdata;

void draw_bars_and_time(float* data, int count, float current_time, float total_time);
void duckwave_playback_callback_with_visual(ma_device* pDevice, void* pOutput, const void* pInput, unsigned int frameCount);
void cleanup();
void init_ncurses();

#endif // ANIMATION_H
