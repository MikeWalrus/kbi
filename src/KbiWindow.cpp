#include "KbiWindow.h"
#include "Player.h"
#include "Ctrl.h"
#include <gtkmm.h>

#define SCIENTIFIC_NOTATION
#undef  SCIENTIFIC_NOTATION

KbiWindow::KbiWindow(Player* p_player)
        :
        kbi_box_top(Gtk::Orientation::VERTICAL, 0),
        kbi_box1(Gtk::Orientation::VERTICAL, 20),
        kbi_box2(Gtk::Orientation::HORIZONTAL, 20),
        kbi_checkbutton_control_voice_limit("Multitude voices"),
        kbi_button_control_play_or_stop("Play"),
        kbi_button_quit("Close"),
        player(p_player),
        kbi_draw(p_player),
        controller(controllers.at("default")(p_player))
{

    //set child widgets
    set_child(kbi_box_top);
    set_title("kbi");
    set_default_size(KbiWindow::kbi_window_width, KbiWindow::kbi_window_height);
    kbi_box_top.append(kbi_box1);
    kbi_box_top.append(kbi_box2);
    kbi_box1.set_size_request(KbiWindow::kbi_window_width, KbiWindow::kbi_window_height - 100);
    kbi_box1.set_expand(true);
    kbi_box1.append(kbi_draw);
    kbi_box2.append(kbi_checkbutton_control_voice_limit);
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

    //set kbi_checkbutton_control_voice_limit
    kbi_checkbutton_control_voice_limit.set_margin(10);
    kbi_checkbutton_control_voice_limit.set_vexpand(false);
    kbi_checkbutton_control_voice_limit.set_hexpand(true);
    kbi_checkbutton_control_voice_limit.set_valign(Gtk::Align::CENTER);
    kbi_checkbutton_control_voice_limit.set_halign(Gtk::Align::FILL);
    kbi_checkbutton_control_voice_limit.set_active(true);
    kbi_checkbutton_control_voice_limit.signal_toggled().connect(
            sigc::mem_fun(*this, &KbiWindow::on_checkbutton_control_voice_limit_toggled));

    setup();
}

void KbiWindow::on_button_control_play_or_stop_clicked()
{
    bool isPlaying = player->toggle();
    if (isPlaying) {
        kbi_button_control_play_or_stop.set_label("Stop");
        add_controller(controller->get_ctrl_key());
    }
    else {
        kbi_button_control_play_or_stop.set_label("Play");
        remove_controller(controller->get_ctrl_key());
    }
}

void KbiWindow::on_button_quit_clicked()
{
    hide();
}

void KbiWindow::on_checkbutton_control_voice_limit_toggled()
{
    bool isMultitude = kbi_checkbutton_control_voice_limit.get_active();
    int voices_number = isMultitude ? 0 : 1;
    player->set_voices_limit(voices_number);
}

KbiWindow::~KbiWindow()
= default;
