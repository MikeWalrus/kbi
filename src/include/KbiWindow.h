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
#include <gtkmm/liststore.h>
#include <functional>
#include <map>
#include <utility>
#include "portaudio.h"
#include "Ctrl.h"
#include "Player.h"
#include "Ctrl.h"
#include "NotesDrawingArea.h"

class Player;

class ModelColumns : public Gtk::TreeModel::ColumnRecord {
public:
    ModelColumns()
    {
        add(names);
    }

    Gtk::TreeModelColumn<std::string> names;
};

struct MyComboBox {
    ModelColumns columns;
    Gtk::ComboBox combo_box;
    Glib::RefPtr<Gtk::ListStore> tree;
    std::function<void(const std::string&)> change_something;

    explicit MyComboBox(std::function<void(const std::string&)> change_something, const vector<Glib::ustring>& names)
            :tree(Gtk::ListStore::create(columns)), change_something(std::move(change_something))
    {
        combo_box.set_model(tree);
        combo_box.set_size_request(300, 50);
        combo_box.pack_start(columns.names);
        combo_box.signal_changed().connect(sigc::mem_fun(*this, &MyComboBox::on_change));
        load_tree_model(names);
    }

private:
    void on_change();

    void load_tree_model(const vector<Glib::ustring>& names);


};

class KbiWindow : public Gtk::Window {
public:
    explicit KbiWindow(Player* p_player);

    ~KbiWindow() override;

    void reset_control(){
        set_control("Linear");
        ctrl_combo_box.combo_box.set_active(0);
    };

protected:
    void on_button_control_play_or_stop_clicked();

    void on_button_quit_clicked();

    void on_switch_control_voices_limit_clicked();

private:

    bool is_playing = false;
    int kbi_window_width = 600;            //decide window's width
    int kbi_window_height = 700;           //decide window's height
    Gtk::Box kbi_vbox_top, kbi_vbox_draw, kbi_hbox_button, kbi_hbox_switch, kbi_hbox_instruments_setting, kbi_hbox_combobox;
    Gtk::Button kbi_button_control_play_or_stop, kbi_button_quit;
    Gtk::Switch kbi_switch_control_voices_limit;

    Player* player;
    NotesDrawingArea kbi_draw;
    typedef function<shared_ptr<Ctrl>(Player*)> CtrlFactoryMethod;
    const std::map<Glib::ustring, CtrlFactoryMethod> controllers = {{"Linear", LinearCtrl::create},
                                                                    {"Score File", [this](auto&& main_window) {
                                                                        return ScoreCtrl::create(
                                                                                std::forward<decltype(main_window)>(main_window), this);
                                                                    }}};
    shared_ptr<Ctrl> controller;

    MyComboBox instrument_combo_box, ctrl_combo_box;
    shared_ptr<Gtk::Settings> settings;

    static void box_append(Gtk::Box& box, Gtk::Widget& widget1, Gtk::Widget& widget2);

    static void box_append(Gtk::Box& box, Gtk::Widget& widget1, Gtk::Widget& widget2, Gtk::Widget& widget3);

    static void init_widget(Gtk::Widget& widget);

    void set_control(const Glib::ustring& name);

};

void setup();//use this to do any initialisation if you want.

#endif //KBI_KBIWINDOW_H
