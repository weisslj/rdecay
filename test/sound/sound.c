#include "SDL.h"

#include "sound.h"

struct {
    SDL_AudioSpec spec;
    Uint8 *sound;               /* Zeiger auf die Audiodaten */
    Uint32 soundlen;            /* Länge der Audiodaten */
    int soundpos;               /* Aktuelle Abspielposition */
} wave;

/* folgende Funktion wird aufgerufen, wenn die 
   Audiohardware weitere Daten benötigt */
void fillerup(void *unused, Uint8 * stream, int len)
{
    Uint8 *waveptr;
    int waveleft;

    /* Zeiger setzten */
    waveptr = wave.sound + wave.soundpos;
    waveleft = wave.soundlen - wave.soundpos;

    /* Abspielen */
    while (waveleft <= len) {
        SDL_MixAudio(stream, waveptr, waveleft, SDL_MIX_MAXVOLUME);
        stream += waveleft;
        len -= waveleft;
        waveptr = wave.sound;
        waveleft = wave.soundlen;
        wave.soundpos = 0;
    }
    SDL_MixAudio(stream, waveptr, len, SDL_MIX_MAXVOLUME);
    wave.soundpos += len;
}

void play_sound(void)
{

/* lade die Audiodatei */
    SDL_LoadWAV("geiger.wav", &wave.spec, &wave.sound, &wave.soundlen);

/* SDL mitteilen, welche Funktion immer wieder 
   aufgerufen werden soll */
    wave.spec.callback = fillerup;

    /* öffne Audioausgabegerät 
       im gewünschten Format */
    if (SDL_OpenAudio(&wave.spec, NULL) < 0) {
        fprintf(stderr, "Kann Audio nicht ausgeben: %s\n", SDL_GetError());
        return (-1);
    }

    /* Pause aufheben, wodurch 
       Wiedergabe gestartet wird */
    SDL_PauseAudio(0);

/* Animation beendet => Sound beenden */
    SDL_CloseAudio();
}
