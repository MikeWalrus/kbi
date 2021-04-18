//
// Created by mike on 4/7/21.

/* changed by zhang on 4/18/21:
 * m_button is changed. (kbi_button_control_play_or_stop)
 * on_button_clicked() is changed. (on_button_control_play_or_stop())
 * set some attributes of the window.
 * increase private: kbi_button_quit, kbi_box_top, kbi_box1, kbi_box2.
 * kbi_box1 and kbi_box2 is in kbi_box_top.
 * kbi_box1 is empty. It is planned to place the unit that prints notes.
 * kbi_button_control_play_or_stop and kbi_button_quit is in kbi_box2.
 * increase protected: on_button_quit_clicked().
 *increase private: kbi_window_width(int), kbi_window_height(int)
 *
 */
//

#ifndef KBI_KBIWINDOW_H
#define KBI_KBIWINDOW_H

#include <gtkmm/window.h>
#include <gtkmm/button.h>
#include <gtkmm/eventcontroller.h>
#include <gtkmm/eventcontrollerkey.h>
#include <gtkmm/box.h>
#include "portaudio.h"
#include "Player.h"

class Player;

class KbiWindow : public Gtk::Window {
public:
    KbiWindow(Player* p_player);

    ~KbiWindow() override;

protected:
    void on_button_control_play_or_stop_clicked();

    bool on_key_pressed(guint keyVal, guint, Gdk::ModifierType state);

    void on_key_released(guint keyVal, guint, Gdk::ModifierType state);

    void on_button_quit_clicked();

private:

    int kbi_window_width = 300;            //decide window's width
    int kbi_window_height = 400;           //decide window's height
    Gtk::Box kbi_box_top, kbi_box1, kbi_box2;
    Gtk::Button kbi_button_control_play_or_stop, kbi_button_quit;

    Player* player;
    Glib::RefPtr<Gtk::EventControllerKey> event_controller_key;

    bool hasGotNote(guint keyVal, const Gdk::ModifierType& state, Player::Note& note) const;
};

void setup();//use this to do any initialisation if you want.

void play(double* output);//run dac! Very very often. Too often in fact. er...

#endif //KBI_KBIWINDOW_H
