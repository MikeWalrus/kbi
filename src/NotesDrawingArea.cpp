#include "NotesDrawingArea.h"
#include <glibmm/main.h>
#include <sstream>

NotesDrawingArea::NotesDrawingArea(Player* p_player)
        :
        player(p_player)
{
    set_content_width(300);
    set_content_height(300);
    set_expand(true);
    set_draw_func(sigc::mem_fun(*this, &NotesDrawingArea::on_draw));
    Glib::signal_timeout().connect(sigc::mem_fun(*this, &NotesDrawingArea::refresh), 100);
    font.set_family("Serif");
    font.set_weight(Pango::Weight::BOLD);
    font.set_size(70000);
    layout = create_pango_layout("");
    layout->set_font_description(font);
}

NotesDrawingArea::~NotesDrawingArea()
= default;

void NotesDrawingArea::on_draw(const Cairo::RefPtr<Cairo::Context>& cr, int width, int height)
{

    // Draw a black rectangle
    cr->set_source_rgb(0.0, 0.0, 0.0);
    draw_rectangle(cr, width, height);

    // and some white text
    cr->set_source_rgb(1.0, 1.0, 1.0);
    draw_text(cr, width, height);

    Cairo::Matrix matrix(1.0, 0.0, 0.0, -1.0, 0.0, height);

    // apply the matrix
    cr->transform(matrix);

}

void NotesDrawingArea::draw_rectangle(const Cairo::RefPtr<Cairo::Context>& cr, int width, int height)
{

    cr->rectangle(0, 0, width, height);
    cr->fill();

}

void NotesDrawingArea::draw_text(const Cairo::RefPtr<Cairo::Context>& cr, int rectangle_width, int rectangle_height)
{

    std::stringstream draw_note;
    auto current_notes = player->get_current_notes();
    for (const auto& note : current_notes) {
        draw_note << get_span_tag(string{static_cast<char>(note.letter)}, "#7ac6ff")
                  << get_span_tag(get_tagged(to_string(note.number), "sub"), "#42f5b3") << " ";
    }

    layout->set_markup(draw_note.str());

    int text_width, text_height;

    //get the text dimensions (it updates the variables -- by reference)
    layout->get_pixel_size(text_width, text_height);

    // Position the text in the middle
    cr->move_to((rectangle_width - text_width)/2., (rectangle_height - text_height)/2.);

    layout->show_in_cairo_context(cr);

    layout->set_markup(get_span_tag(get_tagged(get_tagged("DEMO", "tt"), "small"), "gray"));
    cr->move_to(0, 0);
    layout->show_in_cairo_context(cr);
    layout->set_markup(get_span_tag(get_tagged(player->get_current_instrument(), "i"), "#b3ff70"));
    layout->get_pixel_size(text_width, text_height);
    cr->move_to((rectangle_width - text_width)/2., 0);
    layout->show_in_cairo_context(cr);
}

string NotesDrawingArea::get_span_tag(const string& text, const string& color)
{
    return "<span foreground=\"" + color + "\">" + text + "</span>";
}

string NotesDrawingArea::get_tagged(const string& text, const string& tag)
{
    return "<" + tag + ">" + text + "</" + tag + ">";
}

bool NotesDrawingArea::refresh()
{
    queue_draw();
    return true;
}
