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

        std::string to_string() const;
    };

    struct Keys {
        constexpr static char uri[]{"sample_uri"};
        constexpr static char base_note[]{"base_note"};
        constexpr static char beg[]{"loop_beg"};
        constexpr static char end[]{"loop_end"};
        constexpr static const char* adsr[]{"attack", "decay", "sustain", "release"};
        constexpr static char synth_desc[]{"synth_description"};
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

    [[nodiscard]] vector<pair<Player::Note, bool>> get_current_notes() const;

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

    void clear_voices();

    void load_key_file();

    Glib::RefPtr<Glib::KeyFile> key_file;

    void load_builtin_instruments();

    unique_ptr<Voice> get_prototype(const string& name);

    void save_key_file();

    void load_loop(const string& name, SamplerVoice* sampler_voice);

    array<int, 4> load_adsr(const string& name) const;

    Note load_base_note(const string& name) const;

    unique_ptr<Voice> get_sampler_instrument(const string& name);

    unique_ptr<Voice> get_synth_instrument(const string& name);
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

    void set_adsr(Adsr adsr)
    {
        env.setAttack(adsr[0]);
        env.setDecay(adsr[1]);
        env.setSustain(adsr[2]);
        env.setRelease(adsr[3]);
    }

    explicit Voice(Adsr adsr)
    {
        env.trigger = 0;
        set_adsr(adsr);
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

    [[nodiscard]] bool isOn() const
    {
        return env.trigger;
    }
};

class SynthVoice : public Voice {
private:
    maxiOsc osc;
    vector<maxiOsc> oscillators;
    struct OscSpec {
        double volume;
        double rel_freq;
    };
    vector<OscSpec> osc_specs;

    Voice* new_copy() override
    {
        return new SynthVoice(*this);
    }

public:
    SynthVoice()
            :Voice({10, 50, 100, 1000}) { }

    double output_something() override
    {
        auto fundamental = get_freq();
        double output = 0;
        for (int i = 0; i < oscillators.size(); ++i) {
            auto& spec = osc_specs[i];
            output += 0.3 * spec.volume*oscillators[i].sinewave(spec.rel_freq * fundamental);
        }
        return output;
    }

    explicit SynthVoice(const vector<double>& desc)
            :Voice({})
    {
        oscillators = vector<maxiOsc>{desc.size()/2};
        for (auto it = desc.cbegin(); it != desc.cend(); ++it) {
            osc_specs.push_back({*it, *++it});
            cout << "one" << endl;
        }
        normalise_volumes();
    }

    void normalise_volumes()
    {
        double total_vol = accumulate(osc_specs.begin(), osc_specs.end(), 0.,
                [](double sum, auto& spec) { return sum + spec.volume; });
        for_each(osc_specs.begin(), osc_specs.end(), [total_vol](auto& spec) {
            spec.volume /= total_vol;
        });
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
    Voice* new_copy() override
    {
        return new SamplerVoice(*this);
    }

    maxiSample sample;
    bool shouldTurnOn = false;
    Player::Note base_note;

    Sample_iterator find_zero_cross_near(vector<double>::const_iterator position);

    Sample_iterator find_next_zero_cross(vector<double>::const_iterator iterator);

    [[maybe_unused]] static void display_wave(Sample_iterator begin, Sample_iterator end);

    double how_close(Sample_iterator begin, Sample_iterator end);

    Sample_iterator find_loop_at(Sample_iterator& start, int count);

    long loop_begin{};
    long loop_end{};
    long loop_length{};
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
