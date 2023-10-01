#include "playmp3.h"
#include <ao/ao.h>
#include <stdio.h>
#include <stdlib.h>

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



void play(PlayMP3* mp3) {
    printf("\033[33;1m\u256B6 Playing The Song: \033[35;1m%s\033[m\n", mp3->track);
    while (mpg123_read(mp3->mh, mp3->buffer, mp3->buffer_size, &mp3->done) == MPG123_OK) {
        ao_play(mp3->dev, mp3->buffer, mp3->done);
    }
}
    

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

