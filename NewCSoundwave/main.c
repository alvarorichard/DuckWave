#include <stdio.h>
#include "playmp3.h"
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
#include <SDL2/SDL.h>
#include <stdio.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <ncurses.h>
// para compilar use gcc -g main.c playmp3.c -o playmp3 -lavformat -lavcodec -lavutil -lswresample -lSDL2 -lncurses 

extern volatile sig_atomic_t keep_running;
extern void sigint_handler(int sig);

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <caminho_do_arquivo>\n", argv[0]);
        return 1;
    }

    initscr();
    cbreak();
    noecho();
    nodelay(stdscr, TRUE);  // Define o modo não bloqueante para getch()

    // Restante do código...

    // Finaliza ncurses
    endwin();

    // Configura o manipulador de sinal para SIGINT (Ctrl+C) antes de inicializar o SDL
    signal(SIGINT, sigint_handler);

    char *filename = argv[1];
    PlayAudio audio;

    printf("Inicializando...\n");
    init_PlayAudio(&audio);

    printf("Carregando música: %s\n", filename);
    setMusicAudio(&audio, filename);

    printf("Reproduzindo...\n");
    playAudio(&audio);

    printf("Limpando...\n");
    cleanup_PlayAudio(&audio);

    printf("Fim da reprodução.\n");

    return 0;
}