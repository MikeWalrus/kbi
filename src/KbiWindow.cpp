//
// Created by mike on 4/7/21.
//

#include "KbiWindow.h"
#include "maximilian.h"
#include "portaudio.h"
#include "Player.h"
#include "Ctrl.h"
#include <gtkmm.h>

#define SCIENTIFIC_NOTATION
#undef  SCIENTIFIC_NOTATION

KbiWindow::KbiWindow(Player *p_player)
        :
        kbi_box_top(Gtk::Orientation::VERTICAL, 0),
        kbi_box1(Gtk::Orientation::VERTICAL, 20),
        kbi_box2(Gtk::Orientation::HORIZONTAL, 20),
        kbi_button_control_play_or_stop("Play"),
        kbi_button_quit("Close"),
        player(p_player),
        kbi_draw(p_player),
        controller(controllers.at("default")(p_player)) {

    //set child widgets
    set_child(kbi_box_top);
    set_title("kbi");
    set_default_size(KbiWindow::kbi_window_width, KbiWindow::kbi_window_height);
    kbi_box_top.append(kbi_box1);
    kbi_box_top.append(kbi_box2);
    kbi_box1.set_size_request(KbiWindow::kbi_window_width, KbiWindow::kbi_window_height - 100);
    kbi_box1.set_expand(true);
    kbi_box1.append(kbi_draw);
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

KbiWindow::~KbiWindow()
= default;
