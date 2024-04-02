#include "playmp3.h"
#include <ao/ao.h>
#include <stdio.h>


int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <audio_file>\n", argv[0]);
        return 1;
    }

    char *filename = argv[1];

    PlayAudio audio;
    init_PlayAudio(&audio);
    setMusicAudio(&audio, filename);
    playAudio(&audio);
    cleanup_PlayAudio(&audio);

    return 0;
}