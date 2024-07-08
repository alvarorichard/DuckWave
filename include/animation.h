/**
 * @file animation.h
 * @brief Header file for animation functions and variables used in visualizing
 * audio playback.
 *
 * This file contains the declarations of functions and variables used to create
 * visual representations of audio playback. It includes functions for drawing
 * bars, handling playback callbacks, cleaning up resources, and initializing
 * ncurses.
 */

#ifndef ANIMATION_H
#define ANIMATION_H

#include "../include/duckwave.h"
#include "../include/miniaudio.h"

/**
 * @brief The total duration of the audio track in seconds.
 */
extern float song_duration;

/**
 * @brief The current playback position in seconds.
 */
extern float song_cursor;

/**
 * @brief Flag indicating whether the song has finished playing.
 */
extern bool song_finished;

/**
 * @brief Flag indicating whether the playback is paused.
 */
extern bool is_paused;

/**
 * @brief Structure containing sound data and playback device configuration.
 */
extern DuckWaveSoundData dw_sdata;

/**
 * @brief Draws bars representing audio data and displays the current playback
 * time.
 *
 * This function clears the screen and draws a bar graph to visualize the audio
 * data. It uses smoothed data to create a smoother animation effect. The bars
 * are colored with a gradient effect, and the current playback time and total
 * duration are displayed.
 *
 * @param data An array of floating-point values representing the audio data to
 * be visualized.
 * @param count The number of bars to draw, corresponding to the number of
 * elements in the data array.
 * @param current_time The current playback time in seconds.
 * @param total_time The total duration of the audio track in seconds.
 */
void draw_bars_and_time(float* data, int count, float current_time,
                        float total_time);

/**
 * @brief Callback function for audio playback with visual representation.
 *
 * This function is a callback for audio playback using the MiniAudio library.
 * It reads PCM frames from the audio decoder, processes the audio data to
 * calculate bar heights for visualization, and updates the visual display. It
 * also handles the end of the audio track and pauses the playback if necessary.
 *
 * @param pDevice A pointer to the audio device structure.
 * @param pOutput A pointer to the buffer where the decoded audio frames will be
 * written.
 * @param pInput A pointer to the buffer where the input audio frames would be
 * read from (unused in this function).
 * @param frameCount The number of audio frames to process.
 */
void duckwave_playback_callback_with_visual(ma_device* pDevice, void* pOutput,
                                            const void* pInput,
                                            unsigned int frameCount);

/**
 * @brief Cleans up resources and stops the ncurses environment.
 *
 * This function is registered to be called at program exit to ensure that
 * the ncurses environment is properly cleaned up and the audio device is
 * uninitialized.
 */
void cleanup();

/**
 * @brief Initializes the ncurses environment for visual display.
 *
 * This function sets up the ncurses environment, including color pairs
 * for the gradient effect used in the bar graph visualization.
 */
void init_ncurses();

#endif // ANIMATION_H
