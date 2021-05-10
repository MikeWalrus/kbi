#include "maximilian.h"
#include "Player.h"
#include "membuf.h"
#include "gtkmm.h"

maxiSample SamplerVoice::guitar_sample;

Player::Player(void** stream, double* output)
        :stream(stream), output(output)
{
    load_samples();
}

void Player::load_samples()
{
    try {
        auto sample_bytes = Gio::Resource::lookup_data_global("/kbi/guitar_e3.wav");
        gsize size;
        auto beg = static_cast<const char*>(sample_bytes->get_data(size));
        membuf buf(beg, beg + size);
        istream in(&buf);
        SamplerVoice::guitar_sample.load(in);
    }
    catch (Gio::ResourceError& error) {
        cout << error.what() << endl;
    }
}

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
    static double prev_out = 0;
    double mixed_out = 0;
    scoped_lock<mutex> lock(voices_guard);
    for (auto it = voices.begin(); it != voices.end();) {
        auto voice = it->second;
        // Don't know why voice is sometimes NULL here!
        // Seems it can't be NULL now, since mutex locks this.
        auto voice_output = voice->output();
        if (voice->shouldBeDeleted()) {
            cout << "Delete and erase " << it->first << endl;
            delete voice;
            it = voices.erase(it);
            continue;
        }
        mixed_out += voice_output;
        it++;
    }
    if (abs(prev_out - mixed_out) > 0.1) {
        // cout << prev_out - mixed_out << endl;
        cout << "Before: " << prev_out - mixed_out << endl;
        //mixed_out = (300*prev_out + mixed_out)/301;
        cout << "After: " << prev_out - mixed_out << endl;
    }
    output[0] = mixed_out;//left speaker
    output[1] = output[0];
    prev_out = mixed_out;
}

// calculate the frequency of a note
// Note that this doesn't check if the note is valid.
double Player::noteToFrequency(const Note& note)
{
    int scale[7] = {0, 2, 3, 5, 7, 8, 10};
    int relative_to_A_4 = scale[note.letter - 'A'] + 12*(note.number - 4);
    return 440.0*pow(2.0, ((relative_to_A_4 + note.sharp)/12.0));
}

void Player::note_on(const Note& note)
{
    scoped_lock<mutex> lock(voices_guard);
    auto it = voices.find(note);
    if (!(it == voices.end())) {
        it->second->on(note);
        return;
    }
    if (voices_limit && voices.size() + 1 > voices_limit) {
        auto existing_voice = voices.begin()->second;
        existing_voice->off();
        existing_voice->on(note);
        voices.erase(voices.begin());
        voices[note] = existing_voice;
        return;
    }
    cout << "new voice" << endl;
    auto voice_ptr = new SamplerVoice();
    voice_ptr->on(note);
    voices[note] = voice_ptr;
}

void Player::note_off(const Note& note)
{
    auto it = voices.find(note);
    if (it != voices.end()) {
        (*it).second->off();
    }
}

vector<Player::Note> Player::get_current_notes() const
{
    scoped_lock<mutex> lock(voices_guard);
    vector<Note> ret;
    ret.reserve(voices.size());
    for_each(voices.begin(), voices.end(), [&ret](decltype(*voices.begin()) pair) { ret.push_back(pair.first); });
    return ret;
}

double SamplerVoice::output_something()
{
    auto length = sample.getLength();
    auto begin = length*0.9;
    auto end = length*0.95;
    length = end - begin;
    auto sample_output = sample.play(get_freq()/Player::noteToFrequency({'E', 3})/length*maxiSettings::sampleRate,
            begin,
            end);
    // In order to eliminate the 'audio pops', we should wait until its output reaches 0
    // before going back to the beginning.
    if (shouldTurnOn) {
        if (abs(sample_output) < 0.01) {
            sample.trigger();
            shouldTurnOn = false;
        }
    }
    return sample_output;
}