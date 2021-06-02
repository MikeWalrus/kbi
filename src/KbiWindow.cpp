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
        kbi_hbox_instruments_setting(Gtk::Orientation::HORIZONTAL, 10),
        kbi_hbox_combobox(Gtk::Orientation::HORIZONTAL, 0),
        kbi_button_control_play_or_stop("Play"),
        kbi_button_quit("Close"),
        player(p_player),
        kbi_draw(p_player),
        controller(nullptr),
        instrument_combo_box([p_player](const string& s) { p_player->set_instrument(s); },
                p_player->get_all_instruments()),
        is_playing(false),
        ctrl_combo_box([this](const string& s) { set_control(s); }, get_all_keys(controllers)),
        settings(Gtk::Settings::get_default())
{
    settings->property_gtk_application_prefer_dark_theme().set_value(TRUE);

    //set child widgets
    set_child(kbi_vbox_top);
    set_title("kbi");
    set_default_size(KbiWindow::kbi_window_width, KbiWindow::kbi_window_height);
    box_append(kbi_vbox_top, kbi_vbox_draw, kbi_hbox_instruments_setting, kbi_hbox_button);
    kbi_vbox_draw.set_size_request(KbiWindow::kbi_window_width, KbiWindow::kbi_window_height - 200);
    kbi_vbox_draw.set_expand(true);
    kbi_vbox_draw.append(kbi_draw);
    box_append(kbi_hbox_instruments_setting, kbi_hbox_switch, kbi_hbox_combobox);
    box_append(kbi_hbox_button, kbi_button_control_play_or_stop, kbi_button_quit);
    box_append(kbi_hbox_switch, *Gtk::make_managed<Gtk::Label>("Polyphonic", 0), kbi_switch_control_voices_limit);
    kbi_hbox_switch.set_margin_start(10);
    kbi_hbox_switch.set_margin_end(10);
    box_append(kbi_hbox_combobox, *Gtk::make_managed<Gtk::Label>("Instruments", 0), instrument_combo_box.combo_box);
    box_append(kbi_hbox_combobox, *Gtk::make_managed<Gtk::Label>("Controls", 0), ctrl_combo_box.combo_box);

    //set kbi_button_control_play_or_stop
    init_widget(kbi_button_control_play_or_stop);
    kbi_button_control_play_or_stop.set_size_request(300, 50);
    kbi_button_control_play_or_stop.signal_clicked().connect(
            sigc::mem_fun(*this, &KbiWindow::on_button_control_play_or_stop_clicked));

    //set kbi_button_quit
    init_widget(kbi_button_quit);
    kbi_button_quit.set_size_request(300, 50);
    kbi_button_quit.signal_clicked().connect(
            sigc::mem_fun(*this, &KbiWindow::on_button_quit_clicked));

    //set kbi_switch_control_voices_limit
    init_widget(kbi_switch_control_voices_limit);
    kbi_switch_control_voices_limit.set_hexpand(false);
    kbi_switch_control_voices_limit.set_active(true);
    kbi_switch_control_voices_limit.property_active().signal_changed().connect(
            sigc::mem_fun(*this, &KbiWindow::on_switch_control_voices_limit_clicked));

    init_widget(instrument_combo_box.combo_box);
    init_widget(ctrl_combo_box.combo_box);

    setup();
}

void KbiWindow::box_append(Gtk::Box& box, Gtk::Widget& widget1, Gtk::Widget& widget2)
{
    box.append(widget1);
    box.append(widget2);
}

void KbiWindow::box_append(Gtk::Box& box, Gtk::Widget& widget1, Gtk::Widget& widget2, Gtk::Widget& widget3)
{
    box.append(widget1);
    box.append(widget2);
    box.append(widget3);
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
    is_playing = player->toggle();
    if (is_playing) {
        kbi_button_control_play_or_stop.set_label("Stop");
        add_controller(controller->get_ctrl_key());
    }
    else {
        controller->stop_everything();
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

void KbiWindow::set_control(const Glib::ustring& name)
{
    if (is_playing) {
        is_playing = player->toggle();
        kbi_button_control_play_or_stop.set_label("Play");
        remove_controller(controller->get_ctrl_key());
        controller->stop_everything();
    }
    controller = controllers.at(name)(player);
}

KbiWindow::~KbiWindow()
= default;

void MyComboBox::on_change()
{
    const auto iter = combo_box.get_active();
    if (!iter) {
        return;
    }
    const auto row = *iter;
    if (!row) {
        return;
    }
    string s = row[columns.names];
    functor(s);
}

void MyComboBox::load_tree_model(const vector<Glib::ustring>& names)
{
    for (const auto& name : names) {
        auto row = *(tree->append());
        row[columns.names] = name;
    }
    combo_box.set_active(0);
}
