//
// Created by mike on 4/10/21.
//

#ifndef KBI_PLAYER_H
#define KBI_PLAYER_H

#include <array>
#include "portaudio.h"
#include "maximilian.h"
#include "utilities.h"
class Player {
public:
    struct Note {
        int letter;
        int number;

        bool operator==(Note& note) const
        {
            return letter == note.letter && number == note.number;
        }
        bool operator!=(Note& note) const
        {
            return !(*this==note);
        }
    };
    explicit Player(void** stream, double* output);

    virtual ~Player();
    bool toggle();
    void stop();
    void play(double* output);

    double* output;
    double frequency;

    void note_on(const Note& note);

    void note_off(const Note& note);

    static double noteToFrequency(const Note& note);

private:
    PaStream** stream;
    maxiOsc amySine;//One oscillator - can be called anything. Can be any of the available waveforms.
    PaError err;


    void start() const;

    bool is_playing_a_note = false;
    Note prev_note = {-1, -1};
    int note_count = 0;
};

#endif //KBI_PLAYER_H
