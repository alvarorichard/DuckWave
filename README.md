# CSOUNDWAVE


A basic MP3 player in C using libmpg123 and libao libraries. This project allows you to play, pause, and stop MP3 files from your terminal.

## Features
 * Play MP3 Files: Plays the specified MP3 file.
 * Pause Playback: Toggle pause and play with the spacebar.
 * Basic Error Handling: Handles basic errors during initialization and playback.

 ## Installation

### Prerequisites

Ensure you have the following libraries installed:

* `libmpg123`
* `libao`
  
On Debian/Ubuntu systems, you can install these using:

```bash
sudo apt-get install libmpg123-dev libao-dev
```
### Compiling the Code
```bash
clang main.c playmp3.c -o playmp3 -lmpg123 -lao                      
```
This will create an executable called mp3player in your current directory.

## Usage

1. Add your mp3 file to the project directory.
2. In the main() function of main.c, replace "add you music here .mp3" with your mp3 file name.
3. Compile the code.
4. Run the executable:
```C
./mp3player
```
### Functions

* `init_PlayMP3(PlayMP3* mp3):` Initializes the MP3 player.
* `setMusic(PlayMP3* mp3, char* track)`:: Sets the MP3 file to play.
* `play(PlayMP3* mp3)`: Plays the MP3 file. Toggles pause/play with spacebar.
* `cleanup_PlayMP3(PlayMP3* mp3)`: Cleans up after MP3 playback is done.
* `stop(PlayMP3* mp3)`: Stops MP3 playback.
* `isSpacePressed()`: Checks if the spacebar is pressed (non-blocking).
  

### Note

* Handle errors effectively in your application, especially where resources are initialized and used.
* Avoid memory and resource leaks by freeing unused resources.