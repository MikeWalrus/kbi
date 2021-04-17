//
// Created by mike on 4/7/21.
//

#ifndef KBI_KBIWINDOW_H
#define KBI_KBIWINDOW_H

#include <gtkmm/window.h>
#include <gtkmm/button.h>
#include <gtkmm/eventcontroller.h>
#include <gtkmm/eventcontrollerkey.h>
#include "portaudio.h"
#include "Player.h"

class Player;

class KbiWindow : public Gtk::Window {
public:
    KbiWindow(Player* p_player);

    ~KbiWindow() override;

protected:
    void on_button_clicked();

    bool on_key_pressed(guint keyVal, guint, Gdk::ModifierType state);

    void on_key_released(guint keyVal, guint, Gdk::ModifierType state);

private:
    Gtk::Button m_button;
    Player* player;
    Glib::RefPtr<Gtk::EventControllerKey> event_controller_key;

    bool hasGotNote(guint keyVal, const Gdk::ModifierType& state, Player::Note& note) const;
};

void setup();//use this to do any initialisation if you want.

void play(double* output);//run dac! Very very often. Too often in fact. er...

#endif //KBI_KBIWINDOW_H
