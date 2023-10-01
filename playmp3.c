#include "playmp3.h"
#include <ao/ao.h>
#include <stdio.h>
#include <stdlib.h>
#include <mpg123.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>


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

    if(ch == ' ')
        return 1;

    return 0;
}

void play(PlayMP3* mp3) {
    int isPaused = 0;
    printf("\033[33;1m\u256B6 Playing The Song: \033[35;1m%s\033[m\n", mp3->track);
    while (mpg123_read(mp3->mh, mp3->buffer, mp3->buffer_size, &mp3->done) == MPG123_OK) {
        if (isSpacePressed()) {
            isPaused = !isPaused;
            // Espere um pouco após detectar a tecla espaço para evitar detecções múltiplas.
            usleep(300000); // 300ms
        }

        if (!isPaused) {
            ao_play(mp3->dev, mp3->buffer, mp3->done);
        } else {
            usleep(100000); // 100ms
        }
    }
}


void init_PlayMP3(PlayMP3* mp3){
    ao_initialize();

    mp3->driver = ao_default_driver_id();

    mpg123_init();

    mp3->mh = mpg123_new(NULL, &mp3->err);
    mp3->buffer_size = mpg123_outblock(mp3->mh);
    mp3->buffer = (unsigned char*) malloc(mp3->buffer_size * sizeof(unsigned char));
}

void setMusic(PlayMP3* mp3, char*track){
    mp3->track = track;
    mpg123_open(mp3->mh, mp3->track);
    mpg123_getformat(mp3->mh, &mp3->rate, &mp3->channels, &mp3->encoding);
    mp3->format.bits = mpg123_encsize(mp3->encoding) * 8;
    mp3->format.rate = mp3->rate;
    mp3->format.channels = mp3->channels;
    mp3->format.byte_format = AO_FMT_NATIVE;
    mp3->format.matrix = 0;
    mp3->dev = ao_open_live(mp3->driver, &mp3->format, NULL);

}



// void play(PlayMP3* mp3) {
//     printf("\033[33;1m\u256B6 Playing The Song: \033[35;1m%s\033[m\n", mp3->track);
//     while (mpg123_read(mp3->mh, mp3->buffer, mp3->buffer_size, &mp3->done) == MPG123_OK) {
//         ao_play(mp3->dev, mp3->buffer, mp3->done);
//     }
// }
    

void cleanup_PlayMP3(PlayMP3 *mp3){
    ao_close(mp3->dev);
    mpg123_close(mp3->mh);
    mpg123_delete(mp3->mh);
    mpg123_exit();
    ao_shutdown();
    if (mp3->buffer)
    {
      free(mp3->buffer);
    }
    

}

void stop(PlayMP3* mp3){
    cleanup_PlayMP3(mp3);
    exit(0);
}

