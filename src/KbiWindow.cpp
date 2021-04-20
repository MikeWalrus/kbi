//
// Created by mike on 4/7/21.
//

#include "KbiWindow.h"
#include "maximilian.h"
#include "portaudio.h"
#include "Player.h"
#include <gtkmm.h>

#define SCIENTIFIC_NOTATION
#undef  SCIENTIFIC_NOTATION

KbiWindow::KbiWindow(Player* p_player)
        :
        kbi_box_top(Gtk::Orientation::VERTICAL, 0),
        kbi_box1(Gtk::Orientation::VERTICAL, 20),
        kbi_box2(Gtk::Orientation::HORIZONTAL, 20),
        kbi_button_control_play_or_stop("Play"),
        kbi_button_quit("Close"),
        player(p_player)
{

    //set child widgets
    set_child(kbi_box_top);
    set_title("kbi");
    set_default_size(KbiWindow::kbi_window_width, KbiWindow::kbi_window_height);
    kbi_box_top.append(kbi_box1);
    kbi_box_top.append(kbi_box2);
    kbi_box1.set_size_request(KbiWindow::kbi_window_width, KbiWindow::kbi_window_height - 100);
    kbi_box1.set_expand(true);
    kbi_box2.append(kbi_button_control_play_or_stop);
    kbi_box2.append(kbi_button_quit);

    //set kbi_button_control_play_or_stop
    kbi_button_control_play_or_stop.set_margin(10);
    kbi_button_control_play_or_stop.set_vexpand(false);
    kbi_button_control_play_or_stop.set_hexpand(true);
    kbi_button_control_play_or_stop.set_valign(Gtk::Align::CENTER);
    kbi_button_control_play_or_stop.set_halign(Gtk::Align::FILL);
    kbi_button_control_play_or_stop.signal_clicked().connect(
            sigc::mem_fun(*this, &KbiWindow::on_button_control_play_or_stop_clicked));

    //set kbi_button_quit
    kbi_button_quit.set_margin(10);
    kbi_button_quit.set_vexpand(false);
    kbi_button_quit.set_hexpand(true);
    kbi_button_quit.set_valign(Gtk::Align::CENTER);
    kbi_button_quit.set_halign(Gtk::Align::FILL);
    kbi_button_quit.signal_clicked().connect(
            sigc::mem_fun(*this, &KbiWindow::on_button_quit_clicked));

    event_controller_key = Gtk::EventControllerKey::create();
    event_controller_key->signal_key_pressed().connect(sigc::mem_fun(*this, &KbiWindow::on_key_pressed), false);
    event_controller_key->signal_key_released().connect(sigc::mem_fun(*this, &KbiWindow::on_key_released), true);

    setup();
}

void KbiWindow::on_button_control_play_or_stop_clicked()
{
    bool isPlaying = player->toggle();
    if (isPlaying) {
        kbi_button_control_play_or_stop.set_label("Stop");
        add_controller(event_controller_key);
    }
    else {
        kbi_button_control_play_or_stop.set_label("Play");
        remove_controller(event_controller_key);
    }
}

void KbiWindow::on_button_quit_clicked()
{
    hide();
}

KbiWindow::~KbiWindow()
= default;

void KbiWindow::on_key_released(guint keyVal, guint, Gdk::ModifierType state)
{
    Player::Note note;
    if (hasGotNote(keyVal, state, note)) {
        player->note_off(note);
        // this shows how to find out what notes the Player is playing
        print_current_notes();
    }
}

bool KbiWindow::on_key_pressed(guint keyVal, guint, Gdk::ModifierType state)
{
    int note_number;
    int note_letter;
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
    if (hasGotNote(keyVal, state, note)) {
        player->note_on(note);
        // this shows how to find out what notes the Player is playing
        print_current_notes();
    }
#endif
    return true;
}

bool KbiWindow::hasGotNote(guint keyVal, const Gdk::ModifierType& state, Player::Note& note) const
{
    note.letter = keyVal - GDK_KEY_a + 'a';
    vector<int> notes{'k', 'l', 'a', 's', 'd', 'f', 'j', ';'};
    auto it = find(notes.cbegin(), notes.cend(), note.letter);
    if (it != notes.cend()) {
        note.letter = 'A' + static_cast<int>(it - notes.cbegin());
        note.number = note.letter <= 'B' ? 4 : 3;
        if (it == notes.cend() - 1) {
            note.letter = 'C';
            note.number = 4;
        }
        if ((state & Gdk::ModifierType::CONTROL_MASK) == Gdk::ModifierType::CONTROL_MASK)
            ++note.number;
        if ((state & Gdk::ModifierType::ALT_MASK) == Gdk::ModifierType::ALT_MASK)
            ++note.number;
        return true;
    }
    return false;
}

// Helper function that demonstrate how to find out current notes.
void KbiWindow::print_current_notes()
{
    auto current_notes = player->get_current_notes();
    for (const auto& note : current_notes) {
        cout << note << " ";
    }
    cout << endl;
}


