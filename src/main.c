#include "playmp3.h"
#include <stdio.h>
#include "audio_tools/miniaudio.h"


int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    PlayAudio audio;
    init_PlayAudio(&audio);
    setMusicAudio(&audio, argv[1]);
    playAudio(&audio);
    cleanup_PlayAudio(&audio);

    return 0;
}
