//
// Created by mike on 4/21/21.
//

#include <gtkmm/filechooserdialog.h>
#include <gtkmm/messagedialog.h>
#include <glibmm/main.h>
#include <mutex>
#include "Player.h"
#include "Ctrl.h"
#include "KbiWindow.h"

Ctrl::Ctrl(Player* p_player)
        :ctrl_key(Gtk::EventControllerKey::create()), player(p_player)
{
    Ctrl::ctrl_key->signal_key_pressed().connect(sigc::mem_fun(*this, &Ctrl::on_key_pressed), false);
    Ctrl::ctrl_key->signal_key_released().connect(sigc::mem_fun(*this, &Ctrl::on_key_released), false);
}

bool LinearCtrl::hasGotNote(guint keyVal, const Gdk::ModifierType& state, Player::Note& note)
{
    int key_letter = keyVal - GDK_KEY_a + 'a';
    key_letter = tolower(key_letter);
    static const map<int, Player::Note> notes{{'a', {'C', 3}},
                                              {'s', {'D', 3}},
                                              {'d', {'E', 3}},
                                              {'f', {'F', 3}},
                                              {'j', {'G', 3}},
                                              {'k', {'A', 4}},
                                              {'l', {'B', 4}},
                                              {';', {'C', 4}},
                                              {'q', {'C', 3, true}},
                                              {'w', {'D', 3, true}},
                                              {'r', {'F', 3, true}},
                                              {'u', {'G', 3, true}},
                                              {'i', {'A', 4, true}},
    };
    auto it = notes.find(key_letter);
    if (it != notes.cend()) {
        note = it->second;
        if ((state & Gdk::ModifierType::SHIFT_MASK) == Gdk::ModifierType::SHIFT_MASK)
            ++note.number;
        if ((state & Gdk::ModifierType::ALT_MASK) == Gdk::ModifierType::ALT_MASK)
            ++note.number;
        if ((state & Gdk::ModifierType::CONTROL_MASK) == Gdk::ModifierType::CONTROL_MASK)
            --note.number;
        return true;
    }
    return false;
}

void LinearCtrl::on_key_released(guint keyVal, guint, Gdk::ModifierType state)
{
    Player::Note note;
    if (LinearCtrl::hasGotNote(keyVal, state, note)) {
        for (int i = 2; i <= 6; i++) {
            note.number = i;
            get_player()->note_off(note);
        }
    }
}

bool LinearCtrl::on_key_pressed(guint keyVal, guint, Gdk::ModifierType state)
{
#ifdef SCIENTIFIC_NOTATION
    if (keyVal>=GDK_KEY_a && keyVal<=GDK_KEY_g) {
        note_number = note_letter <= 'B' ? 4 : 3;
        note_letter = static_cast<int>(keyVal) - GDK_KEY_a + 'A';
        if ((state & Gdk::ModifierType::CONTROL_MASK) == Gdk::ModifierType::CONTROL_MASK)
            ++note_number;
        if ((state & Gdk::ModifierType::ALT_MASK) == Gdk::ModifierType::ALT_MASK)
            ++note_number;
        cout << static_cast<char>(note_letter) << note_number;
        player->noteOn({note_letter, note_number});
    }
#else
    Player::Note note;
    if (LinearCtrl::hasGotNote(keyVal, state, note)) {
        get_player()->note_on(note);
    }
#endif
    return true;
}

ScoreCtrl::ScoreCtrl(Player* p_player, KbiWindow* window)
        :Ctrl(p_player), main_window(window)
{
    ask_for_file(main_window);
}

void ScoreCtrl::ask_for_file(KbiWindow* window)
{
    auto dialog = new Gtk::FileChooserDialog("Please choose a file",
            Gtk::FileChooser::Action::OPEN);
    dialog->set_transient_for(*window);
    dialog->set_modal(true);
    dialog->add_button("_Cancel", Gtk::ResponseType::CANCEL)->set_margin(5);
    dialog->add_button("_Open", Gtk::ResponseType::OK)->set_margin(5);
    dialog->signal_response().connect(sigc::bind(
            sigc::mem_fun(*this, &ScoreCtrl::on_file_dialog_response), dialog));

    dialog->show();
}

void ScoreCtrl::on_file_dialog_response(int response_id, Gtk::FileChooserDialog* dialog)
{
    switch (response_id) {
    case Gtk::ResponseType::OK: {
        filename = dialog->get_file()->get_path();
        try {
            parse_score(filename);
        }
        catch (runtime_error& error) {
            report_error(error.what());
        }
        break;
    }
    case Gtk::ResponseType::CANCEL: {
        if (filename.empty())
            main_window->reset_control();
        break;
    }
    default: {
        break;
    }
    }
    delete dialog;
}

void ScoreCtrl::parse_score(const string& file_name)
{
    stop_everything();
    score.clear();
    ifstream file(file_name);
    string line;
    int line_num = 0;
    while (getline(file, line)) {
        ++line_num;
        auto comment_begin = line.find("//"); // get rid of comments
        if (comment_begin != string::npos)
            line.erase(comment_begin, line.length());
        if (line.empty()) // skip empty lines
            continue;
        line.erase(line.begin(),
                find_if(line.cbegin(), line.cend(), [](auto c) { return isalpha(c); })); // skip leading spaces
        string op, args;
        parse_line(line, args, op);
        try {
            Operation operation = func_table.at(op);
            Instruction i{operation, args, line_num};
            score.push_back(i);
        }
        catch (...) {
            throw std::runtime_error(get_error_msg(line_num, "invalid operation: " + op));
        }
    }
    current_line = score.cbegin();
}

void
ScoreCtrl::parse_line(const string& line, string& args,
        string& op)
{
    auto after_op = line.find(' ');
    op = line.substr(0, after_op);
    if (after_op == string::npos)
        args = "";
    else
        args = string(line.cbegin() + static_cast<long>(after_op), line.cend());
}

void ScoreCtrl::set_tempo(const string& arg)
{
    int temp_tempo;
    try {
        temp_tempo = stoi(arg);
        tempo = temp_tempo;
    }
    catch (exception& error) {
        throw std::runtime_error("tempo invalid");
    }
}

void ScoreCtrl::note_on(const string& args)
{
    Player::Note note = getNote(args);
    get_player()->note_on(note);
}

void ScoreCtrl::note_off(const string& args)
{
    Player::Note note = getNote(args);
    get_player()->note_off(note);
}

Player::Note ScoreCtrl::getNote(const string& args)
{
    stringstream ss(args);
    Player::Note note;
    ss >> note;
    return note;
}

void ScoreCtrl::run_score()
{
    has_started = true;
    if (!timeout.connected())
        timeout = Glib::signal_timeout().connect(sigc::mem_fun(*this, &ScoreCtrl::tick), tick_interval);
}

bool ScoreCtrl::tick()
{
    if (!has_started)
        return true;
    execute_delayed_tasks();
    if (wait_ticks) {
        wait_ticks--;
        return true;
    }
    for (; current_line != score.cend() && wait_ticks == 0; ++current_line)
        execute(*current_line);
    return true;
}

void ScoreCtrl::execute_delayed_tasks()
{
    for (auto it = tasks.begin(); it != tasks.end();) {
        if (!--(it->wait_ticks)) {
            it->task();
            it = tasks.erase(it);
            continue;
        }
        ++it;
    }
}

void ScoreCtrl::execute(const ScoreCtrl::Instruction& instruction)
{
    try {
        (this->*instruction.op)(instruction.args);
    }
    catch (std::runtime_error& error) {
        string msg = get_error_msg(current_line->line, error.what());
        report_error(msg);
    }
}

void ScoreCtrl::report_error(const string& msg)
{
    m = make_unique<Gtk::MessageDialog>(msg);
    m->set_title("Syntax error in " + filename);
    m->set_size_request(700, -1);
    m->signal_response().connect([this](int id) {
        if (id == Gtk::ResponseType::OK) {
            {
                has_started = false;
                m->hide();
                main_window->reset_control();
            }
        }
    });
    m->set_transient_for(*main_window);
    m->show();
}

void ScoreCtrl::wait(const string& args)
{
    stringstream ss(args);
    double beats;
    if (!(ss >> beats)) {
        throw std::runtime_error("wait beats invalid");
    }
    wait(beats);
}

void ScoreCtrl::wait(double beats) { wait_ticks = get_ticks(beats); }

int ScoreCtrl::get_ticks(double beats) const
{
    if (!tempo) {
        throw std::runtime_error("tempo not set");
    }
    auto ms_per_beat = 60*1000/tempo;
    auto ticks_per_beat = ms_per_beat/tick_interval;
    return static_cast<int>(beats*ticks_per_beat);
}

string ScoreCtrl::get_error_msg(int line_num, const string& what)
{
    stringstream ss;
    ss << "line " << line_num << ": " << what;
    return ss.str();
}

void ScoreCtrl::note_auto(const string& args)
{
    stringstream ss(args);
    Player::Note note;
    double beats_before_off;
    if (!(ss >> note >> beats_before_off))
        throw std::runtime_error("note: invalid args");
    string modifier;
    if (!(ss >> modifier) || modifier != "and")
        wait(beats_before_off);
    auto player = get_player();
    player->note_on(note);
    int ticks_before_off = get_ticks(beats_before_off);
    if (!ticks_before_off)
        ticks_before_off = 1;
    Task new_task{ticks_before_off, [player, note]() { player->note_off(note); }};
    tasks.push_back(new_task);
}

void ScoreCtrl::begin_loop(const string& args)
{
    stringstream ss(args);
    int times = 1;
    bool infinite = false;
    if (!(ss >> times))
        infinite = true;
    loops.push({times, current_line, infinite});
}

void ScoreCtrl::end_loop(const string& args)
{
    auto& current_loop = loops.top();
    if (!current_loop.infinite)
        --current_loop.times_left;
    auto jump_to = current_loop.loop_beg;
    if (!current_loop.times_left) {
        loops.pop();
        return;
    }
    if (jump_to == score.cend())
        throw std::runtime_error("cannot loop back");
    current_line = jump_to;
}

void ScoreCtrl::stop_everything()
{
    has_started = false;
    get_player()->clear_voices();
    timeout.disconnect();
}

bool ScoreCtrl::on_key_pressed(guint keyVal, guint, Gdk::ModifierType state)
{
    switch (keyVal) {
    case GDK_KEY_s:
        run_score();
        break;
    case GDK_KEY_o:
        ask_for_file(main_window);
        break;
    case GDK_KEY_p:
        pause_score();
        break;
    case GDK_KEY_r:
        restart_score();
    default:
        return true;
    }
    return true;
}

void ScoreCtrl::pause_score()
{
    has_started = false;
    stop_everything();
}

void ScoreCtrl::restart_score()
{
    current_line = score.cbegin();
    loops = {};
    stop_everything();
    run_score();
}




