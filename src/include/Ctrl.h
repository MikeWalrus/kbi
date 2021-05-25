//
// Created by mike on 4/21/21.
//

#ifndef KBI_CTRL_H
#define KBI_CTRL_H

#include <gtkmm/eventcontrollerkey.h>
#include <gtkmm/filechooserdialog.h>
#include "Player.h"
#include <functional>

class KbiWindow;

class Ctrl {
public:
    [[nodiscard]] const Glib::RefPtr<Gtk::EventControllerKey>& get_ctrl_key() const
    {
        return ctrl_key;
    }

    explicit Ctrl(Player* p_player);

    [[nodiscard]] Player* get_player() const
    {
        return player;
    }

    virtual void stop_everything() { };

private:
    Player* player;
    Glib::RefPtr<Gtk::EventControllerKey> ctrl_key;

    virtual void on_key_released(guint keyVal, guint, Gdk::ModifierType state) = 0;

    virtual bool on_key_pressed(guint keyVal, guint, Gdk::ModifierType state) = 0;


};

class LinearCtrl : public Ctrl {
public:
    explicit LinearCtrl(Player* p_player)
            :Ctrl(p_player) { }

    static shared_ptr<Ctrl> create(Player* p_player)
    {
        return make_shared<LinearCtrl>(p_player);
    }

private:

    static bool hasGotNote(guint keyVal, const Gdk::ModifierType& state, Player::Note& note);

    bool on_key_pressed(guint keyVal, guint, Gdk::ModifierType state) override;

    void on_key_released(guint keyVal, guint, Gdk::ModifierType state) override;
};

class ScoreCtrl : public Ctrl {
public:
    explicit ScoreCtrl(Player* p_player, KbiWindow* window);

    static shared_ptr<Ctrl> create(Player* p_player, KbiWindow* window)
    {
        return make_shared<ScoreCtrl>(p_player, window);
    }

    void stop_everything() override;

private:
    typedef void (ScoreCtrl::* Operation)(const string&);

    struct Instruction {
        Operation op;
        string args;
        int line;
    };

    KbiWindow* main_window;
    std::vector<Instruction> score;
    vector<Instruction>::const_iterator current_line;
    std::map<std::string, Operation> func_table{
            {"tempo",   &ScoreCtrl::set_tempo},
            {"noteon",  &ScoreCtrl::note_on},
            {"noteoff", &ScoreCtrl::note_off},
            {"wait",    &ScoreCtrl::wait},
            {"note",    &ScoreCtrl::note_auto}
    };
    int tempo{};
    bool has_started = false;
    int wait_ticks = 0;
    int tick_interval = 100;
    struct Task {
        int wait_ticks;
        function<void()> task;
    };
    std::vector<Task> tasks;

    bool on_key_pressed(guint keyVal, guint, Gdk::ModifierType state) override
    {
        if (keyVal == GDK_KEY_s) {
            run_score();
        }
        return true;
    };

    void on_key_released(guint keyVal, guint, Gdk::ModifierType state) override { };

    void on_file_dialog_response(int response_id, Gtk::FileChooserDialog* dialog);

    void parse_score(const string& file_name);

    void set_tempo(const string& arg);

    void note_on(const string& args);

    void wait(const string& args);

    void note_auto(const string& args);

    bool tick();

    void execute(const Instruction& instruction);

    int get_ticks(double beats) const;

    void run_score();

    static string get_error_msg(int line_num, const string& what);

    [[nodiscard]] static Player::Note getNote(const string& args);

    void note_off(const string& args);

    void execute_delayed_tasks();

    void wait(double beats);
};

#endif //KBI_CTRL_H
