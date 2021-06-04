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
#include <array>
#include <glibmm/keyfile.h>
#include "portaudio.h"
#include "maximilian.h"
#include "utilities.h"

class Voice;

class SamplerVoice;

/**
 * A class that allocates and manages Voices and provide the audio output.
 */
class Player {
public:
    /**
     * A struct that represents musical notes.
     */
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

        /**
         * Get the frequency this note.
         * Note that this doesn't check whether the note has valid fields.
         * @return frequency
         */
        [[nodiscard]] double get_frequency() const
        {
            int scale[7] = {0, 2, 3, 5, 7, 8, 10};
            int relative_to_A_4 = scale[letter - 'A'] + 12*(number - 4);
            return 440.0*pow(2.0, ((relative_to_A_4 + sharp)/12.0));
        }

        friend std::istream& operator>>(std::istream& is, Note& note);

#ifndef __clang__

        // This is C++20 magic! But won't compile with clang 10
        auto operator<=>(const Note&) const = default;

#else

        bool operator<(const Note& note) const
        {
            tuple<int, int, bool> lhs = {letter, number, sharp};
            tuple<int, int, bool> rhs = {note.letter, note.number, note.sharp};
            return lhs < rhs;
        }

#endif

        friend ostream& operator<<(ostream& os, const Note& note)
        {
            os << static_cast<char>(note.letter) << note.number << (note.sharp ? '#' : ' ');
            return os;
        }

        [[nodiscard]] std::string to_string() const;
    };

    /**
     * Literals to use as keys in Glib::KeyFile.
     *
     * Acts like a enum for strings.
     */
    struct Keys {
        constexpr static char uri[]{"sample_uri"};
        constexpr static char base_note[]{"base_note"};
        constexpr static char beg[]{"loop_beg"};
        constexpr static char end[]{"loop_end"};
        constexpr static const char* adsr[]{"attack", "decay", "sustain", "release"};
    };

    explicit Player(void** stream, double* output);

    virtual ~Player();

    Player(const Player&) = delete;

    Player& operator=(const Player&) = delete;

    /**
     * toggle the playing status of Player (on to off / off to on)
     * @return whether is playing.
     */
    bool toggle();

    void stop();

    void play();

    double* output;

    /**
     * Start playing a note
     */
    void note_on(const Note& note);

    /**
     * Stop a specific note. Nothing will happen if the note isn't currently on.
     */
    void note_off(const Note& note);

    /**
     * Clear all the voices so that all current note are discarded.
     */
    void clear_voices();

    /**
     * Specify 'how polyphonic' the Player should be.
     * @param voices_number maximum number of voices. (0 if no restriction)
     */
    void set_voices_limit(int voices_number);

    [[nodiscard]] vector<pair<Player::Note, bool>> get_current_notes() const;

    /**
     * Change current instrument.
     * @param name name of a instrument
     */
    void set_instrument(const string& name);

    const string& get_current_instrument();

    std::vector<Glib::ustring> get_all_instruments();

private:
    PaStream** stream;
    unique_ptr<Voice> voice_prototype;
    map<Note, Voice*> voices;
    mutable mutex voices_guard;
    decltype(voices)::size_type voices_limit = 0; // Maximum number of voices. 0 means no limits.
    string current_instrument_name;

    void start() const;

    void load_instruments();

    void load_key_file();

    Glib::RefPtr<Glib::KeyFile> key_file;

    void load_builtin_instruments();

    unique_ptr<Voice> get_prototype(const string& name);

    void save_key_file();

    void load_loop(const string& name, SamplerVoice* sampler_voice);

    array<int, 4> load_adsr(const string& name) const;

    Note load_base_note(const string& name) const;
};

/**
 * The base class of all Voices.
 */
class Voice {
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
            freq = note.get_frequency();
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

    [[nodiscard]] bool isOn() const
    {
        return env.trigger;
    }

private:
    maxiEnv env;
    double volume{};
    double freq{};

    virtual Voice* new_copy() = 0;
};

class SynthVoice : public Voice {
public:
    SynthVoice()
            :Voice({10, 50, 100, 1000}) { }

    double output_something() override
    {
        return 0.1*osc.sinewave(get_freq());
    }

private:
    maxiOsc osc;

    Voice* new_copy() override
    {
        return new SynthVoice(*this);
    }
};

class SamplerVoice : public Voice {
public:
    explicit SamplerVoice(const Adsr& adsr, const Player::Note& sample_base_note)
            :Voice(adsr), base_note(sample_base_note)
    {
    };

    typedef vector<double>::const_iterator Sample_iterator;
    typedef array<long, 2> Loop;

    void load(const string& sample_uri);

    [[nodiscard]] Loop find_loop();

    double output_something() override;

    void turn_on_something() override
    {
        shouldTurnOn = true;
    }

    void set_loop(const array<long, 2>& loop);

private:
    maxiSample sample;
    bool shouldTurnOn = false;
    Player::Note base_note;
    long loop_begin{};
    long loop_end{};
    long loop_length{};

    Voice* new_copy() override
    {
        return new SamplerVoice(*this);
    }

    Sample_iterator find_zero_cross_near(vector<double>::const_iterator position);

    Sample_iterator find_next_zero_cross(vector<double>::const_iterator iterator);

    [[maybe_unused]] static void display_wave(Sample_iterator begin, Sample_iterator end);

    double how_close(Sample_iterator begin, Sample_iterator end);

    Sample_iterator find_loop_at(Sample_iterator& start, int count);
};

template<class T>
std::vector<typename T::key_type> get_all_keys(const T& m)
{
    std::vector<typename T::key_type> ret;
    ret.reserve(m.size());
    for_each(m.begin(), m.end(), [&ret](typename T::value_type pair) { ret.push_back(pair.first); });
    return ret;
}

#endif //KBI_PLAYER_H
