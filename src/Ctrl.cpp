//
// Created by mike on 4/21/21.
//

#include <gtkmm/filechooserdialog.h>
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
        auto filename = dialog->get_file()->get_path();
        break;
    }
    case Gtk::ResponseType::CANCEL: {
        std::cout << "Cancel clicked." << std::endl;
        main_window->reset_control();
        break;
    }
    default: {
        std::cout << "Unexpected button clicked." << std::endl;
        break;
    }
    }
    delete dialog;
}
