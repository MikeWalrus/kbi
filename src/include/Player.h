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
        bool sharp = false;

        bool operator==(const Note& note) const
        {
            return letter == note.letter && number == note.number;
        }

        bool operator!=(const Note& note) const
        {
            return !(*this == note);
        }

#ifndef __clang__

        // This is C++20 magic! But won't compile with clang
        auto operator<=>(const Note&) const = default;

#else
        bool operator<(const Note& note) const
        {
            tuple<int,int,bool> lhs = {letter, number, sharp};
            tuple<int,int,bool> rhs = {note.letter, note.number, note.sharp};
            return lhs < rhs;
        }
#endif

        friend ostream& operator<<(ostream& os, const Note& note)
        {
            os << static_cast<char>(note.letter) << note.number << (note.sharp ? '#' : ' ');
            return os;
        }
    };

    explicit Player(void** stream, double* output);

    virtual ~Player();

    Player(const Player&) = delete;

    Player& operator=(const Player&) = delete;

    bool toggle();

    void stop();

    void play();

    double* output;

    void note_on(const Note& note);

    void note_off(const Note& note);

    static double noteToFrequency(const Note& note);

    [[nodiscard]] vector<Note> get_current_notes() const;

private:
    PaStream** stream;
    map<Note, Voice*> voices;
    mutable mutex voices_guard;
    decltype(voices)::size_type voices_limit = 0; // Maximum number of voices. 0 means no limits.

    void start() const;

};

class Voice {
private:
    maxiOsc osc;
    maxiEnv env;
    double freq{};
    double volume{};

public:
    Voice();

    virtual double output();

    virtual void off();

    virtual void on(const Player::Note& note);

    [[nodiscard]] bool shouldBeDeleted() const;

    void set_adsr(int attack, int decay, int sustain, int release);

protected:
    [[nodiscard]] double get_freq() const;

public:
    double get_volume() const;
};

class SamplerVoice : public Voice {
public:
    SamplerVoice();

    double output() override;

    void off() override;

    void on(const Player::Note& note) override;

    static maxiSample mother_of_samples; // TODO: change its name
private:
    maxiSample sample;
    bool hasTriggered = false;
};

#endif //KBI_PLAYER_H
