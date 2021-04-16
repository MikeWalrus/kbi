//
// Created by mike on 4/10/21.
//
#include <maxiSynths.h>
#include "maximilian.h"
#include "Player.h"

double Voice::output()
{
    volume = env.adsr(1., env.trigger);
    return osc.sinewave(freq)*volume;
}

Voice::Voice()
{
    env.setAttack(5);
    env.setDecay(50);  // Needs to be at least 1
    env.setSustain(100);
    env.setRelease(1000);
}

void Voice::on(int midi_pitch)
{
    env.trigger = 1;
    freq = Player::midiToFrequency(midi_pitch);
}

void Voice::on(const Player::Note& note)
{
    env.trigger = 1;
    freq = Player::noteToFrequency(note);
}

void Voice::off()
{
    env.trigger = 0;
}

bool Voice::shouldBeDeleted()
{
    return (volume<0.1) && env.trigger==0;
}

Player::Player(void** stream, double* output)
        :stream(stream), output(output) { }

Player::~Player()
{
    Pa_CloseStream(*stream);
}

// Return whether is playing.
bool Player::toggle()
{
    if (Pa_IsStreamActive(*stream)) {
        stop();
        return false;
    }
    else {
        start();
        return true;
    }
}

void Player::start() const { reportIfPaFail(Pa_StartStream(*stream)); }

void Player::stop()
{
    reportIfPaFail(Pa_StopStream(*stream));
    for_each(voices.begin(), voices.end(), [](std::pair<const int, Voice*>& pair) { delete pair.second; });
    voices.clear();
}

void setup()
{//some inits
}

void Player::play(double* output)
{
    double mixed_out = 0;
    for (auto it = voices.begin(); it!=voices.end();) {
        Voice* voice;
        voice = it->second;
        // Don't know why voice is sometimes NULL here!
        while (voice==nullptr) {
            cout << "Oh Noooooooooo! voice is NULL again!\n!\n!\n!\n";
            voice = it->second;
            if (voice !=nullptr)
            {
                cout << "voice is good to use now?!\n";
            }
        }
        auto voice_output = voice->output();
        if (voice->shouldBeDeleted()) {
            cout << "Delete and erase." << it->first << endl;
            delete voice;
            it = voices.erase(it);
            continue;
        }
        mixed_out += voice_output;
        it++;
    }
    output[0] = mixed_out;//left speaker
    output[1] = output[0];

}

double Player::noteToFrequency(const Note& note)
{
    return 440.0*pow(2.0, (relative_to_A_4(note)/12.0));
}

double Player::midiToFrequency(int pitch)
{
    return 440.0*pow(2.0, ((pitch - 69.0)/12.0));
}

int Player::relative_to_A_4(const Note& note)
{
    int scale[7] = {0, 2, 3, 5, 7, 8, 10};
    return scale[note.letter - 'A'] + 12*(note.number - 4);
}

void Player::note_on(const Note& note)
{
    int pitch = note.to_midi_pitch();
    cout << pitch << endl;
    auto it = voices.find(pitch);
    if (it==voices.end()) {
        cout << "new voice" << endl;
        auto voice_ptr = new Voice();
        voice_ptr->on(pitch);
        voices[pitch] = voice_ptr;
    }
    else {
        cout << "using existing voice" << endl;
        it->second->on(pitch);
    }

    /*
    if (note!=prev_note) {
        cout << (char) note.letter << note.number << endl;
        ++note_count;
        cout << "count added: " << note_count << endl;
        prev_note = note;
        frequency = noteToFrequency(note);
        cout << " frequency " << frequency << endl;
        is_playing_a_note = true;
    }
     */
}

void Player::note_off(const Note& note)
{
    int pitch = note.to_midi_pitch();
    cout << "Request to stop" << pitch << endl;
    auto it = voices.find(pitch);
    if (it!=voices.end()) {
        (*it).second->off();
    }

    /*
    --note_count;
    cout << "count subtracted: " << note_count << endl;
    if (note_count<=0) {
        note_count = 0;
        prev_note = {-1};
        is_playing_a_note = false;
    }
     */
}
