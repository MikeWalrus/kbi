//
// Created by mike on 4/21/21.
//

#ifndef KBI_CTRL_H
#define KBI_CTRL_H

#include <gtkmm/eventcontrollerkey.h>
#include "Player.h"

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

private:
    Player* player;
    Glib::RefPtr<Gtk::EventControllerKey> ctrl_key;

    virtual void on_key_released(guint keyVal, guint, Gdk::ModifierType state) = 0;

    virtual bool on_key_pressed(guint keyVal, guint, Gdk::ModifierType state) = 0;


};

class Controller : public Ctrl {
public:
    explicit Controller(Player* p_player)
            :Ctrl(p_player)
    {

    }

    static shared_ptr<Ctrl> create(Player* p_player);

private:

    bool hasGotNote(guint keyVal, const Gdk::ModifierType& state, Player::Note& note);

    bool on_key_pressed(guint keyVal, guint, Gdk::ModifierType state) override;

    void on_key_released(guint keyVal, guint, Gdk::ModifierType state) override;
};

#endif //KBI_CTRL_H
