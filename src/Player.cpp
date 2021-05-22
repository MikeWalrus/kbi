#include <stdexcept>
#include <algorithm>
#include "maximilian.h"
#include "Player.h"
#include "membuf.h"
#include "gtkmm.h"

Player::Player(void** stream, double* output)
        :stream(stream), output(output)
{
    auto data_dir = Gio::File::create_for_path(Glib::get_user_data_dir() + "/kbi");
    if (!data_dir->query_exists())
        data_dir->make_directory();
    load_instruments();
    set_instrument(*get_all_instruments().begin());
}

Player::~Player()
{
    stop();
    Pa_CloseStream(*stream);
    save_key_file();
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
// Note that this doesn't check whether the note is valid.
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

vector<pair<Player::Note, bool>> Player::get_current_notes() const
{
    scoped_lock<mutex> lock(voices_guard);
    vector<pair<Note, bool>> ret;
    ret.reserve(voices.size());
    for_each(voices.begin(), voices.end(), [&ret](auto i) { ret.push_back({i.first, i.second->isOn()}); });
    return ret;
}

void Player::load_instruments()
{
    load_key_file();
    load_builtin_instruments();
}

void Player::load_builtin_instruments()
{
    char dir_prefix[] = "/kbi/instruments/";
    auto children_str = Gio::Resource::enumerate_children_global(dir_prefix);
    for (auto str : children_str) {
        Voice::Adsr adsr{10, 10, 100, 1000};
        string dir = string("resource://") + dir_prefix + str;
        replace(str.begin(), str.end(), '.', ' ');
        replace(str.begin(), str.end(), '_', ' ');
        stringstream ss(str);
        string name;
        Note base_note;
        ss >> name;
        ss >> base_note;

        key_file->set_string(name, "sample_uri", dir);
        key_file->set_string(name, "base_note", base_note.to_string());
        key_file->set_comment(name, "builtin instrument: " + name);
    }
}

double SamplerVoice::output_something()
{
    auto freq = get_freq()/Player::noteToFrequency(base_note)/static_cast<double>(loop_length)*sample.mySampleRate;
    auto sample_output = sample.play(freq, static_cast<double>(loop_begin), static_cast<double>(loop_end));
    // In order to eliminate the 'audio pops', we should wait until its output reaches 0
    // before going back to the beginning.
    if (shouldTurnOn) {
        if (abs(sample_output) < 0.1) {
            sample.trigger();
            shouldTurnOn = false;
        }
    }
    return sample_output;
}

void SamplerVoice::load(const string& sample_uri)
{
    using namespace Gio;
    try {
        auto sample_file = File::create_for_uri(sample_uri);
        auto size = sample_file->query_info()->get_size();
        auto buffer = new char[size];
        gsize loaded_size;
        sample_file->load_contents(buffer, loaded_size);
        membuf buf(buffer, buffer + size);
        istream in(&buf);
        sample.load(in);
        delete[] buffer;
    }
    catch (ResourceError& error) {
        cout << error.what() << endl;
        throw;
    }
}

SamplerVoice::Loop SamplerVoice::find_loop()
{
    auto start = find_zero_cross_near(
            std::next(sample.amplitudes->cbegin(), static_cast<long>(sample.amplitudes->size()*2/3)));
    int count = 20;
    Sample_iterator best_end;
    try {
        best_end = find_loop_at(start, count);
    }
    catch (std::runtime_error& error) {
        best_end = find_loop_at(start, count/2);
    }
    auto begin = start - sample.amplitudes->cbegin();
    auto end = best_end - sample.amplitudes->cbegin();
    return {begin, end};
}

SamplerVoice::Sample_iterator SamplerVoice::find_loop_at(SamplerVoice::Sample_iterator& start, int count)
{
    auto next = start;
    auto best = next;
    double badness_min = 1;
    for (int i = 0; i < count; ++i) {
        next = find_next_zero_cross(next);
        double badness = how_close(start, next);
        cout << i << ": Badness: " << badness << endl;
        if (badness < badness_min) {
            badness_min = badness;
            best = next;
        }
    }
    cout << "Best Effort: " << badness_min << endl;
    return best;
}

SamplerVoice::Sample_iterator SamplerVoice::find_zero_cross_near(Sample_iterator position)
{
    return adjacent_find(position, sample.amplitudes->cend(),
            [](auto prev, auto next) { return signbit(prev) != signbit(next); });
}

SamplerVoice::Sample_iterator SamplerVoice::find_next_zero_cross(Sample_iterator iterator)
{
    if (iterator + 100 > sample.amplitudes->cend())
        return iterator;
    return find_zero_cross_near(iterator + 100);
}

[[maybe_unused]] void SamplerVoice::display_wave(SamplerVoice::Sample_iterator begin, SamplerVoice::Sample_iterator end)
{
    for (auto it = begin; it < end; it += 3) {
        for (int i = 0; i < (*it + 1)*50; ++i) {
            cout << "*";
        }
        cout << endl;
    }
}

double SamplerVoice::how_close(SamplerVoice::Sample_iterator begin, SamplerVoice::Sample_iterator end)
{
    double ret = 0;
    if (2*(end - begin) + end > sample.amplitudes->cend())
        throw std::runtime_error("Reached the end!");
    for (auto this_loop = begin, next_loop = end; this_loop != end; ++this_loop, ++next_loop) {
        ret += abs(*this_loop - *next_loop);
    }
    return ret/static_cast<double>(end - begin);
}

void SamplerVoice::set_loop(const array<long, 2>& loop)
{
    loop_begin = loop[0];
    loop_end = loop[1];
    loop_length = loop_end - loop_begin;
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

std::string Player::Note::to_string() const
{
    return static_cast<char>(letter) + std::to_string(number) + (sharp ? "#" : " ");
}

void Player::set_instrument(const string& name)
{
    scoped_lock<mutex> lock(voices_guard);
    clear_voices();
    voice_prototype = get_prototype(name);
    current_instrument_name = name;
}

std::vector<Glib::ustring> Player::get_all_instruments()
{
    return key_file->get_groups();
}

void Player::set_voices_limit(int voices_number)
{
    Player::voices_limit = voices_number;
}

const string& Player::get_current_instrument()
{
    return current_instrument_name;
}

void Player::load_key_file()
{
    using namespace Glib;
    key_file = KeyFile::create();
    string full_path;
    try {
        key_file->load_from_data_dirs("kbi/instruments.conf", full_path);
    }
    catch (KeyFileError& error) {
        // if instruments.conf doesn't exist, load the default file
        auto default_file = Gio::Resource::lookup_data_global("/kbi/instruments.conf");
        gsize size;
        auto default_file_ptr = static_cast<const char*>(default_file->get_data(size));
        string default_file_str{default_file_ptr, default_file_ptr + default_file->get_size()};
        key_file->load_from_data(default_file_str);
    }
}

void Player::save_key_file() { key_file->save_to_file(Glib::get_user_data_dir() + "/kbi/instruments.conf"); }

unique_ptr<Voice> Player::get_prototype(const string& name)
{
    using namespace Glib;
    if (key_file->has_key(name, Keys::uri)) {
        auto uri = key_file->get_string(name, Keys::uri);
        Player::Note base_note = load_base_note(name);
        Voice::Adsr adsr = load_adsr(name);
        auto* sampler_voice = new SamplerVoice(adsr, base_note);
        sampler_voice->load(uri);
        load_loop(name, sampler_voice);

        return unique_ptr<Voice>(sampler_voice);
    }
    return unique_ptr<Voice>(new SynthVoice);
}

Player::Note Player::load_base_note(const string& name) const
{
    Note base_note;
    auto base_note_str = key_file->get_string(name, Keys::base_note);
    stringstream ss(base_note_str);
    ss >> base_note;
    return base_note;
}

Voice::Adsr Player::load_adsr(const string& name) const
{
    Voice::Adsr default_adsr{10, 20, 100, 1000};
    Voice::Adsr adsr;
    for (int i = 0; i < 4; ++i) {
        if (!key_file->has_key(name, Keys::adsr[i])) {
            adsr = default_adsr;
            cout << "Default adsr" << endl;
            break;
        }
        adsr[i] = key_file->get_integer(name, Keys::adsr[i]);
    }
    return adsr;
}

void Player::load_loop(const string& name, SamplerVoice* sampler_voice)
{
    SamplerVoice::Loop loop;
    if (!key_file->has_key(name, Keys::beg)) {
        loop = sampler_voice->find_loop();
        key_file->set_int64(name, Keys::beg, loop[0]);
        key_file->set_int64(name, Keys::end, loop[1]);
    }
    else {
        loop[0] = key_file->get_int64(name, Keys::beg);
        loop[1] = key_file->get_int64(name, Keys::end);
    }
    sampler_voice->set_loop(loop);
}

