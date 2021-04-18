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

void Voice::on(const Player::Note& note)
{
    env.trigger = 1;
    freq = Player::noteToFrequency(note);
}

void Voice::off()
{
    env.trigger = 0;
}

bool Voice::shouldBeDeleted() const
{
    return (volume < 0.0001) && env.trigger == 0;
}

Player::Player(void** stream, double* output)
        :stream(stream), output(output) { }

Player::~Player()
{
    stop();
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
    for_each(voices.begin(), voices.end(), [](decltype(*(voices.begin()))& pair) { delete pair.second; });
    voices.clear();
}

void setup()
{//some inits
}

void Player::play()
{
    double mixed_out = 0;
    scoped_lock<mutex> lock(voices_guard);
    for (auto it = voices.begin(); it != voices.end();) {
        Voice* voice;
        voice = it->second;
        // Don't know why voice is sometimes NULL here!
        // Seems it can't be NULL now, since mutex locks this.
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
    int scale[7] = {0, 2, 3, 5, 7, 8, 10};
    int relative_to_A_4 = scale[note.letter - 'A'] + 12*(note.number - 4);
    return 440.0*pow(2.0, (relative_to_A_4/12.0));
}

void Player::note_on(const Note& note)
{
    scoped_lock<mutex> lock(voices_guard);
    // int pitch = note.to_midi_pitch();
    //cout << pitch << endl;
    auto it = voices.find(note);
    if (!(it == voices.end())) {
        it->second->on(note);
        return;
    }
    if (voices_limit && voices.size() + 1 > voices_limit) {
        auto existing_voice = voices.begin()->second;
        existing_voice->on(note);
        voices.erase(voices.begin());
        voices[note] = existing_voice;
        return;
    }
    cout << "new voice" << endl;
    auto voice_ptr = new Voice();
    voice_ptr->on(note);
    voices[note] = voice_ptr;
}

void Player::note_off(const Note& note)
{
    cout << "Request to stop " << note << endl;
    auto it = voices.find(note);
    if (it != voices.end()) {
        (*it).second->off();
    }
}
