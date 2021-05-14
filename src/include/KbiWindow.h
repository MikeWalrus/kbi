//
// Created by mike on 4/7/21.
//

#ifndef KBI_KBIWINDOW_H
#define KBI_KBIWINDOW_H

#include <gtkmm/window.h>
#include <gtkmm/button.h>
#include <gtkmm/switch.h>
#include <gtkmm/eventcontroller.h>
#include <gtkmm/eventcontrollerkey.h>
#include <gtkmm/box.h>
#include <gtkmm/treemodel.h>
#include <gtkmm/combobox.h>
#include <functional>
#include <map>
#include <gtkmm/liststore.h>
#include "portaudio.h"
#include "Ctrl.h"
#include "Player.h"
#include "Ctrl.h"
#include "NotesDrawingArea.h"

class Player;

class KbiWindow : public Gtk::Window {
public:
    explicit KbiWindow(Player* p_player);

    ~KbiWindow() override;

protected:
    void on_button_control_play_or_stop_clicked();

    void on_button_quit_clicked();

    void on_switch_control_voices_limit_clicked();

    void on_combobox();

private:

    int kbi_window_width = 600;            //decide window's width
    int kbi_window_height = 700;           //decide window's height
    Gtk::Box kbi_vbox_top, kbi_vbox_draw, kbi_hbox_button, kbi_hbox_switch, kbi_hbox_instruments_setting, kbi_hbox_combobox;
    Gtk::Button kbi_button_control_play_or_stop, kbi_button_quit;
    Gtk::Switch kbi_switch_control_voices_limit;

    Player* player;
    NotesDrawingArea kbi_draw;
    typedef function<shared_ptr<Ctrl>(Player*)> CtrlFactoryMethod;
    const std::map<string, CtrlFactoryMethod> controllers = {{"default", Controller::create}};
    shared_ptr<Ctrl> controller;

    class ModelColumns : public Gtk::TreeModel::ColumnRecord {
    public:
        ModelColumns()
        {
            add(kbi_instruments_name);
        }

        Gtk::TreeModelColumn<std::string> kbi_instruments_name;
    };

    ModelColumns kbi_columns;
    Gtk::ComboBox kbi_combobox_instruments;
    Glib::RefPtr<Gtk::ListStore> kbi_ref_treemodel;
    shared_ptr<Gtk::Settings> settings;

    static void init_widget(Gtk::Widget& widget);

    void load_instrument_tree_model();
};

void setup();//use this to do any initialisation if you want.

void play(double* output);//run dac! Very very often. Too often in fact. er...

#endif //KBI_KBIWINDOW_H
