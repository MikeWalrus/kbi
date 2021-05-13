//
// Created by mike on 4/10/21.
//

#ifndef KBI_PLAYER_H
#define KBI_PLAYER_H


#include <map>
#include <maxiSynths.h>
#include <mutex>
#include <memory>
#include <utility>
#include "portaudio.h"
#include "maximilian.h"
#include "utilities.h"

class Voice;

class Instrument;

class Player {
public:
    struct Note {
        int letter{};
        int number{};
        bool sharp = false;

        bool operator==(const Note& note) const
        {
            return letter == note.letter && number == note.number;
        }

        bool operator!=(const Note& note) const
        {
            return !(*this == note);
        }

        friend std::istream& operator>>(std::istream& is, Note& note);

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

    void set_voices_limit(int voices_number);

    static double noteToFrequency(const Note& note);

    [[nodiscard]] vector<Note> get_current_notes() const;

    void set_instrument(const string& name);

    void next_instrument();

    const string& get_current_instrument();

    vector<string> get_all_instruments();

private:
    PaStream** stream;
    unique_ptr<Voice> voice_prototype;
    map<Note, Voice*> voices;
    mutable mutex voices_guard;
    decltype(voices)::size_type voices_limit = 0; // Maximum number of voices. 0 means no limits.
    map<std::string, Instrument> instruments;

    void start() const;

    void load_instruments();

    void clear_voices();

    decltype(instruments.cbegin()) current_instrument;

    void set_instrument(decltype(instruments.cbegin()) iterator);

};

class Voice {
private:
    maxiEnv env;
    double volume{};
    double freq{};

    virtual Voice* new_copy() = 0;

public:

    Voice* clone()
    {
        return new_copy();
    }

    typedef array<int, 4> Adsr;

    explicit Voice(Adsr adsr)
    {
        env.trigger = 0;
        env.setAttack(adsr[0]);
        env.setDecay(adsr[1]);
        env.setSustain(adsr[2]);
        env.setRelease(adsr[3]);
    }

    virtual ~Voice() = default;

    virtual double output_something() = 0;

    double output()
    {
        volume = env.adsr(1., env.trigger);
        return volume*output_something();
    }

    virtual void turn_on_something() { }

    void on(const Player::Note& note)
    {
        if (!env.trigger) {
            env.trigger = 1;
            freq = Player::noteToFrequency(note);
            turn_on_something();
        }
    }

    void off()
    {
        env.trigger = 0;
    }

    [[nodiscard]] bool shouldBeDeleted() const
    {
        return (volume < 0.0001) && env.trigger == 0;
    }

    [[nodiscard]] double get_freq() const
    {
        return freq;
    }
};

class SynthVoice : public Voice {
private:
    maxiOsc osc;

    Voice* new_copy() override
    {
        return new SynthVoice(*this);
    }

public:
    SynthVoice()
            :Voice({10, 50, 100, 1000}) { }

    double output_something() override
    {
        return 0.1*osc.sinewave(get_freq());
    }
};

class SamplerVoice : public Voice {
public:
    explicit SamplerVoice(const Adsr& adsr, const Player::Note& sample_base_note)
            :Voice(adsr), base_note(sample_base_note)
    {
    };

    typedef vector<double>::const_iterator Sample_iterator;

    void load(const string& sample_dir);

    void find_loop();

    double output_something() override;

    void turn_on_something() override
    {
        shouldTurnOn = true;
    }

private:
    Voice* new_copy() override
    {
        return new SamplerVoice(*this);
    }

    maxiSample sample;
    bool shouldTurnOn = false;
    Player::Note base_note;

    Sample_iterator find_zero_cross_near(vector<double>::const_iterator position);

    Sample_iterator find_next_zero_cross(vector<double>::const_iterator iterator);

    static void display_wave(Sample_iterator begin, Sample_iterator end);

    double how_close(Sample_iterator begin, Sample_iterator end);

    Sample_iterator find_best_loop(Sample_iterator& start, int count);

    long loop_begin;
    long loop_end;
    long loop_length;
};

class Instrument {
public:
    Instrument(const Voice::Adsr& adsr, string sample_dir, Player::Note base_note)
            :adsr(adsr), sample_dir(std::move(sample_dir)), base_note(base_note) { }

    Instrument() = default;

    [[nodiscard]] unique_ptr<Voice> get_prototype() const;

private:
    Voice::Adsr adsr;
    std::string sample_dir;
    Player::Note base_note;
};

template<class T>
std::vector<typename T::key_type> get_all_keys(const T& m)
{
    std::vector<typename T::key_type> ret;
    ret.reserve(m.size());
    for_each(m.begin(), m.end(), [&ret](class T::value_type pair) { ret.push_back(pair.first); });
    return ret;
}

#endif //KBI_PLAYER_H
