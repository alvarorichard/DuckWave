#include "playmp3.h"
#include <ao/ao.h>
#include <stdio.h>

int main(){
    PlayMP3 mp3;
    init_PlayMP3(&mp3);
    //add your mp3 file to the project folder and change the name here
    setMusic(&mp3, "slayer-cult.mp3");
    play(&mp3);
    cleanup_PlayMP3(&mp3);
    return 0;
}