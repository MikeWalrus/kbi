#include <stdexcept>
#include "maximilian.h"
#include "Player.h"
#include "membuf.h"
#include "gtkmm.h"

maxiSample SamplerVoice::guitar_sample;

Player::Player(void** stream, double* output)
        :stream(stream), output(output)
{
    load_instruments();
    instruments[string("Sine Wave")] = Instrument({10, 10, 100, 1000}, "sine", Player::Note());
    set_instrument(instruments.begin()->first);
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
    clear_voices();
}

void Player::clear_voices()
{
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
            delete voice;
            it = voices.erase(it);
            continue;
        }
        mixed_out += voice_output;
        it++;
    }
    if (abs(prev_out - mixed_out) > 0.1) {
        cerr << "Pop!\n";
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
    auto voice_ptr = voice_prototype->clone();
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
    return get_all_keys(voices);
}

void Player::load_instruments()
{
    char dir_prefix[] = "/kbi/instruments/";
    auto children_str = Gio::Resource::enumerate_children_global(dir_prefix);
    for (auto str : children_str) {
        Voice::Adsr adsr{10, 10, 100, 1000};
        std::string dir = dir_prefix + str;
        replace(str.begin(), str.end(), '.', ' ');
        replace(str.begin(), str.end(), '_', ' ');
        stringstream ss(str);
        std::string name;
        Player::Note base_note;
        ss >> name;
        ss >> base_note;
        instruments[name] = {adsr, dir, base_note};
    }
}

double SamplerVoice::output_something()
{
    auto length = sample.getLength();
    auto begin = length/10*9;
    auto end = length/100*92;
    length = end - begin;
    auto frequ = get_freq()/Player::noteToFrequency(base_note)/length*sample.mySampleRate;
    auto sample_output = sample.play(frequ, static_cast<double>(begin), static_cast<double>(end));
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

void SamplerVoice::load(const string& sample_dir)
{
    try {
        auto sample_bytes = Gio::Resource::lookup_data_global(sample_dir);
        gsize size;
        auto beg = static_cast<const char*>(sample_bytes->get_data(size));
        membuf buf(beg, beg + size);
        istream in(&buf);
        sample.load(in);
    }
    catch (Gio::ResourceError& error) {
        cout << error.what() << endl;
        throw;
    }
}

std::istream& operator>>(istream& is, Player::Note& note)
{
    is >> std::ws;
    int note_letter = is.get();
    int note_number;
    bool is_sharp = false;
    note_letter = toupper(note_letter);
    if (note_letter > 'G' || note_letter < 'A')
        throw runtime_error("Invalid note notation");
    is >> note_number;
    if (note_number < 0 || note_number > 10)
        throw runtime_error("Invalid note notation");
    int next_char = is.get();
    if (next_char == '#') {
        is_sharp = true;
    }
    else
        is.putback(static_cast<char>(next_char));
    note = {note_letter, note_number, is_sharp};
    return is;
}

void Player::set_instrument(const string& name)
{
    auto instrument = instruments.find(name);
    set_instrument(instrument);
}

std::vector<string> Player::get_all_instruments()
{
    return get_all_keys(instruments);
}

unique_ptr<Voice> Instrument::get_prototype() const
{
    Voice* voice;
    if (sample_dir.find("/kbi") == string::npos) {
        voice = new SynthVoice();
    }
    else {
        auto samplerVoice = new SamplerVoice(adsr, base_note);
        samplerVoice->load(sample_dir);
        voice = samplerVoice;
    }
    return unique_ptr<Voice>(voice);
}

void Player::set_voices_limit(int voices_number)
{
    Player::voices_limit = voices_number;
}

void Player::next_instrument()
{
    if (++current_instrument == instruments.cend())
        current_instrument = instruments.cbegin();
    set_instrument(current_instrument);
}

void Player::set_instrument(map<string, Instrument>::const_iterator iterator)
{
    scoped_lock<mutex> lock(voices_guard);
    clear_voices();
    voice_prototype = iterator->second.get_prototype();
    current_instrument = iterator;
}

const string& Player::get_current_instrument()
{
    return current_instrument->first;
}

