//
// Created by mike on 4/10/21.
//

#ifndef KBI_PLAYER_H
#define KBI_PLAYER_H

#include <map>
#include <maxiSynths.h>
#include <mutex>
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
            return letter == note.letter && number == note.number;
        }

        bool operator!=(Note& note) const
        {
            return !(*this == note);
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

    void play();

    double* output;

    void note_on(const Note& note);

    void note_off(const Note& note);

    static double midiToFrequency(int pitch)
    {
        return 440.0*pow(2.0, ((pitch - 69.0)/12.0));
    }

    unsigned long get_voices_limit() const
    {
        return voices_limit;
    }

    void set_voices_limit(unsigned long voices_limit)
    {
        Player::voices_limit = voices_limit;
    }

private:
    PaStream** stream;
    map<int, Voice*> voices;
    mutex voices_guard;
    map<int, Voice*>::size_type voices_limit = 1; // Maximum number of voices. 0 means no limits.

    void start() const;

    static int relative_to_A_4(const Note& note);

};

class Voice {
private:
    maxiOsc osc;
    maxiEnv env;
    double freq{};
    double volume{};
    Player::Note current_note{};
    int pitch{};

public:
    Voice();

    double output();

    void off();

    void on(int midi_pitch);

    [[nodiscard]] bool shouldBeDeleted() const;
};

#endif //KBI_PLAYER_H
