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
        :m_button("Play"), player(p_player)
{
    m_button.set_margin(100);
    m_button.signal_clicked().connect(sigc::mem_fun(*this, &KbiWindow::on_button_clicked));
    auto controller_key = Gtk::EventControllerKey::create();
    auto controller_focus = Gtk::EventControllerFocus::create();
    controller_key->signal_key_pressed().connect(sigc::mem_fun(*this, &KbiWindow::on_key_pressed), false);
    controller_key->signal_key_released().connect(sigc::mem_fun(*this, &KbiWindow::on_key_released), true);
    controller_focus->signal_leave().connect(sigc::mem_fun(*player, &Player::stop));
    add_controller(controller_key);
    set_child(m_button);
    setup();
}

void KbiWindow::on_button_clicked()
{
    bool isPlaying = player->toggle();
    m_button.set_label(isPlaying ? "Stop" : "Play");
}

KbiWindow::~KbiWindow()
= default;

void KbiWindow::on_key_released(guint keyVal, guint, Gdk::ModifierType state)
{

    Player::Note note;
    if (hasGotNote(keyVal, state, note))
        player->note_off(note);
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
    if (hasGotNote(keyVal, state, note))
        player->note_on(note);
#endif
    return true;
}

bool KbiWindow::hasGotNote(guint keyVal, const Gdk::ModifierType& state, Player::Note& note) const
{
    note.letter = keyVal - GDK_KEY_a + 'a';
    vector<int> notes{'k', 'l', 'a', 's', 'd', 'f', 'j', ';'};
    auto it = find(notes.cbegin(), notes.cend(), note.letter);
    if (it!=notes.cend()) {
        note.letter = 'A' + static_cast<int>(it - notes.cbegin());
        note.number = note.letter<='B' ? 4 : 3;
        if (it==notes.cend() - 1) {
            note.letter = 'C';
            note.number = 4;
        }
        if ((state & Gdk::ModifierType::CONTROL_MASK)==Gdk::ModifierType::CONTROL_MASK)
            ++note.number;
        if ((state & Gdk::ModifierType::ALT_MASK)==Gdk::ModifierType::ALT_MASK)
            ++note.number;
        return true;
    }
    return false;
}


