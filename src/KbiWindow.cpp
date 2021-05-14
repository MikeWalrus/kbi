#include "KbiWindow.h"
#include "Player.h"
#include "Ctrl.h"
#include <gtkmm.h>

#define SCIENTIFIC_NOTATION
#undef  SCIENTIFIC_NOTATION

KbiWindow::KbiWindow(Player* p_player)
        :
        kbi_vbox_top(Gtk::Orientation::VERTICAL, 0),
        kbi_vbox_draw(Gtk::Orientation::VERTICAL, 20),
        kbi_hbox_button(Gtk::Orientation::HORIZONTAL, 20),
        kbi_hbox_switch(Gtk::Orientation::HORIZONTAL, 0),
        kbi_hbox_combobox(Gtk::Orientation::HORIZONTAL, 0),
        kbi_button_control_play_or_stop("Play"),
        kbi_button_quit("Close"),
        player(p_player),
        kbi_draw(p_player),
        controller(controllers.at("default")(p_player)),
        settings(Gtk::Settings::get_default())
{
    settings->property_gtk_application_prefer_dark_theme().set_value(TRUE);

    //set child widgets
    set_child(kbi_vbox_top);
    set_title("kbi");
    set_default_size(KbiWindow::kbi_window_width, KbiWindow::kbi_window_height);
    kbi_vbox_top.append(kbi_vbox_draw);
    kbi_vbox_top.append(kbi_hbox_button);
    kbi_vbox_draw.set_size_request(KbiWindow::kbi_window_width, KbiWindow::kbi_window_height - 100);
    kbi_vbox_draw.set_expand(true);
    kbi_vbox_draw.append(kbi_draw);
    kbi_hbox_button.append(kbi_hbox_switch);
    kbi_hbox_button.append(kbi_hbox_combobox);
    kbi_hbox_button.append(kbi_button_control_play_or_stop);
    kbi_hbox_button.append(kbi_button_quit);
    kbi_hbox_switch.append(*Gtk::make_managed<Gtk::Label>("Polyphonic", 0));
    kbi_hbox_switch.append(kbi_switch_control_voices_limit);
    kbi_hbox_switch.set_margin_start(10);
    kbi_hbox_switch.set_margin_end(10);
    kbi_hbox_combobox.append(*Gtk::make_managed<Gtk::Label>("Instruments", 0));
    kbi_hbox_combobox.append(kbi_combobox_instruments);

    //set kbi_button_control_play_or_stop
    init_widget(kbi_button_control_play_or_stop);
    kbi_button_control_play_or_stop.signal_clicked().connect(
            sigc::mem_fun(*this, &KbiWindow::on_button_control_play_or_stop_clicked));

    //set kbi_button_quit
    init_widget(kbi_button_quit);
    kbi_button_quit.signal_clicked().connect(
            sigc::mem_fun(*this, &KbiWindow::on_button_quit_clicked));

    //set kbi_switch_control_voices_limit
    init_widget(kbi_switch_control_voices_limit);
    kbi_switch_control_voices_limit.set_hexpand(false);
    kbi_switch_control_voices_limit.set_active(true);
    kbi_switch_control_voices_limit.property_active().signal_changed().connect(
            sigc::mem_fun(*this, &KbiWindow::on_switch_control_voices_limit_clicked));

    //set kbi_combobox_instruments
    init_widget(kbi_combobox_instruments);
    //create the tree model
    kbi_ref_treemodel = Gtk::ListStore::create(kbi_columns);
    kbi_combobox_instruments.set_model(kbi_ref_treemodel);
    kbi_combobox_instruments.pack_start(kbi_columns.kbi_instruments_name);
    kbi_combobox_instruments.signal_changed().connect(
            sigc::mem_fun(*this, &KbiWindow::on_combobox));
    //fill the tree model
    load_instrument_tree_model();

    setup();
}

void KbiWindow::load_instrument_tree_model()
{
    auto instruments = player->get_all_instruments();
    for (const auto& name : instruments) {
        auto row = *(kbi_ref_treemodel->append());
        row[kbi_columns.kbi_instruments_name] = name;
    }
    kbi_combobox_instruments.set_active(0);
}

void KbiWindow::init_widget(Gtk::Widget& widget)
{
    widget.set_margin(10);
    widget.set_vexpand(false);
    widget.set_hexpand(true);
    widget.set_valign(Gtk::Align::CENTER);
    widget.set_halign(Gtk::Align::FILL);
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

void KbiWindow::on_combobox()
{
    const auto iter = kbi_combobox_instruments.get_active();
    if (!iter) {
        return;
    }
    const auto row = *iter;
    if (!row) {
        return;
    }
    string instrument_name = row[kbi_columns.kbi_instruments_name];
    player->set_instrument(instrument_name);
}

KbiWindow::~KbiWindow()
= default;
