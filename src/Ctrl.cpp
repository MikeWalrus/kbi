//
// Created by mike on 4/21/21.
//

#include <Player.h>
#include "Ctrl.h"

Ctrl::Ctrl(Player* p_player)
        :ctrl_key(Gtk::EventControllerKey::create()), player(p_player)
{
    Ctrl::ctrl_key->signal_key_pressed().connect(sigc::mem_fun(*this, &Ctrl::on_key_pressed), false);
    Ctrl::ctrl_key->signal_key_released().connect(sigc::mem_fun(*this, &Ctrl::on_key_released), false);
}

// Helper function that demonstrate how to find out current notes.
void Controller::print_current_notes()
{
    auto current_notes = get_player()->get_current_notes();
    for (const auto& note : current_notes) {
        cout << note << " ";
    }
    cout << endl;
}

bool Controller::hasGotNote(guint keyVal, const Gdk::ModifierType& state, Player::Note& note)
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
        // this shows how to find out what notes the Player is playing
        print_current_notes();
        return true;
    }
    return false;
}

void Controller::on_key_released(guint keyVal, guint, Gdk::ModifierType state)
{
    Player::Note note;
    if (Controller::hasGotNote(keyVal, state, note)) {
        get_player()->note_off(note);
    }
}

bool Controller::on_key_pressed(guint keyVal, guint, Gdk::ModifierType state)
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
    if (Controller::hasGotNote(keyVal, state, note)) {
        get_player()->note_on(note);
    }
#endif
    return true;
}

shared_ptr<Ctrl> Controller::create(Player* p_player)
{
    return make_shared<Controller>(p_player);
}
