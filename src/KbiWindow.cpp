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
        kbi_box_switch(Gtk::Orientation::HORIZONTAL, 0),
        kbi_button_control_play_or_stop("Play"),
        kbi_button_quit("Close"),
        button_next_instrument("Next Instrument"),
        player(p_player),
        kbi_draw(p_player),
        controller(controllers.at("default")(p_player)),
        settings(Gtk::Settings::get_default())
{
    settings->property_gtk_application_prefer_dark_theme().set_value(TRUE);

    //set child widgets
    set_child(kbi_box_top);
    set_title("kbi");
    set_default_size(KbiWindow::kbi_window_width, KbiWindow::kbi_window_height);
    kbi_box_top.append(kbi_box1);
    kbi_box_top.append(kbi_box2);
    kbi_box1.set_size_request(KbiWindow::kbi_window_width, KbiWindow::kbi_window_height - 100);
    kbi_box1.set_expand(true);
    kbi_box1.append(kbi_draw);
    kbi_box2.append(kbi_box_switch);
    kbi_box2.append(kbi_button_control_play_or_stop);
    kbi_box2.append(kbi_button_quit);
    kbi_box2.append(button_next_instrument);
    kbi_box_switch.append(*Gtk::make_managed<Gtk::Label>("Polyphonic", 0));
    kbi_box_switch.append(kbi_switch_control_voices_limit);
    kbi_box_switch.set_margin_start(10);
    kbi_box_switch.set_margin_end(10);

    //set kbi_button_control_play_or_stop
    init_button(kbi_button_control_play_or_stop);
    kbi_button_control_play_or_stop.signal_clicked().connect(
            sigc::mem_fun(*this, &KbiWindow::on_button_control_play_or_stop_clicked));

    //set kbi_button_quit
    init_button(kbi_button_quit);
    kbi_button_quit.signal_clicked().connect(
            sigc::mem_fun(*this, &KbiWindow::on_button_quit_clicked));

    init_button(button_next_instrument);
    button_next_instrument.signal_clicked().connect(sigc::mem_fun(*this, &KbiWindow::next_instrument));

    //set kbi_switch_control_voices_limit
    kbi_switch_control_voices_limit.set_margin(10);
    kbi_switch_control_voices_limit.set_vexpand(false);
    kbi_switch_control_voices_limit.set_hexpand(false);
    kbi_switch_control_voices_limit.set_valign(Gtk::Align::CENTER);
    kbi_switch_control_voices_limit.set_halign(Gtk::Align::FILL);
    kbi_switch_control_voices_limit.set_active(true);
    kbi_switch_control_voices_limit.property_active().signal_changed().connect(
            sigc::mem_fun(*this, &KbiWindow::on_switch_control_voices_limit_clicked));

    setup();
}

void KbiWindow::init_button(Gtk::Button& bu)
{
    bu.set_margin(10);
    bu.set_vexpand(false);
    bu.set_hexpand(true);
    bu.set_valign(Gtk::Align::CENTER);
    bu.set_halign(Gtk::Align::FILL);
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

void KbiWindow::on_switch_control_voices_limit_clicked()
{
    bool isPolyphonic = kbi_switch_control_voices_limit.get_active();
    int voices_number;
    voices_number = isPolyphonic ? 0 : 1;
    player->set_voices_limit(voices_number);
}

void KbiWindow::next_instrument()
{
    player->next_instrument();
}

KbiWindow::~KbiWindow()
= default;
