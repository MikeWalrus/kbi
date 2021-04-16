//
// Created by mike on 4/10/21.
//

#ifndef KBI_PLAYER_H
#define KBI_PLAYER_H

#include <map>
#include <maxiSynths.h>
#include "portaudio.h"
#include "maximilian.h"
#include "utilities.h"

class Voice;

class Player {
public:
    struct Note {
        int letter;
        int number;

        bool operator==(Note& note) const
        {
            return letter==note.letter && number==note.number;
        }

        bool operator!=(Note& note) const
        {
            return !(*this==note);
        }

        [[nodiscard]] int to_midi_pitch() const
        {
            return Player::relative_to_A_4(*this) + 69;
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

    static double midiToFrequency(int pitch);

private:
    PaStream** stream;
    map<int, Voice*> voices;

    void start() const;

    static int relative_to_A_4(const Note& note);

};

class Voice {
private:
    maxiOsc osc;
    maxiEnv env;
    double freq;
    double volume;
    Player::Note current_note;
    int pitch;

public:
    Voice();

    double output();

    void on(const Player::Note& note);

    void off();

    void on(int midi_pitch);

    bool shouldBeDeleted();
};

#endif //KBI_PLAYER_H
