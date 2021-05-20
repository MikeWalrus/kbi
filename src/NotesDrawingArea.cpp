#include "NotesDrawingArea.h"
#include <glibmm/main.h>
#include <sstream>

NotesDrawingArea::NotesDrawingArea(Player* p_player)
        :player(p_player)
{
    set_content_width(300);
    set_content_height(300);
    set_expand(true);
    set_draw_func(sigc::mem_fun(*this, &NotesDrawingArea::on_draw));
    Glib::signal_timeout().connect(sigc::mem_fun(*this, &NotesDrawingArea::refresh), 60);
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

    cr->set_source_rgb(0.0, 0.0, 0.0);
    cr->rectangle(0, 0, width, height);
    cr->fill();

}

void NotesDrawingArea::draw_text(const shared_ptr<Cairo::Context> cr, int rectangle_width, int rectangle_height)
{
    int text_width, text_height;
    layout->set_markup(get_span_tag(get_tagged(player->get_current_instrument(), "i"), "#b3ff70"));
    layout->get_pixel_size(text_width, text_height);
    double instrument_x = (rectangle_width - text_width)/2.;
    double instrument_y = 20;
    rect_instrument.draw_rect_fill(cr, text_width, text_height, instrument_x, instrument_y, 0.5);
    cr->move_to(instrument_x, instrument_y);
    layout->show_in_cairo_context(cr);
    std::stringstream draw_note;
    auto current_notes = player->get_current_notes();
    for (const auto& note : current_notes) {
        draw_note << get_span_tag(string{static_cast<char>(note.letter)}, "#7ac6ff")
                  << get_span_tag(get_tagged(to_string(note.number) + (note.sharp ? "#" : ""), "sub"), "#42f5b3")
                  << " ";
    }
    string notes = draw_note.str();
    if (!notes.empty()) {
        notes.pop_back();
    }
    layout->set_markup(notes);


    //get the text dimensions (it updates the variables -- by reference)
    layout->get_pixel_size(text_width, text_height);
    int count = text_width/(rectangle_width - 100);
    if (count) {
        break_line(notes, count);
        layout->set_markup(notes);
        layout->get_pixel_size(text_width, text_height);
    }

    double notes_x = (rectangle_width - text_width)/2.;
    double notes_y = (rectangle_height - text_height)/2.;
    rect_notes.draw_border(cr, text_width, text_height, notes_x, notes_y, 0.2);
    // Position the text in the middle
    cr->move_to(notes_x, notes_y);

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

void NotesDrawingArea::break_line(string& markup_text, int count)
{
    cout << count << endl;
    for (int i = 0; i < count; ++i) {
        auto break_pos = markup_text.find("> <", markup_text.size()/(count + 1)*(i + 1));
        if (break_pos != string::npos) {
            markup_text.insert(break_pos + 1, "\n");
        }
    }
}

void
ElasticRect::draw_border(Cairo::RefPtr<Cairo::Context> context, double target_width, double target_height, double x,
        double y, double factor)
{
    update_rect(context, target_width, target_height, x, y, factor);
    stroke(context);
}

void
ElasticRect::draw_rect_fill(Cairo::RefPtr<Cairo::Context> context, double target_width, double target_height, double x, double y,
        double factor)
{
    update_rect(context, target_width, target_height, x, y, factor);
    fill_rect(context);
}

void
ElasticRect::update_rect(Cairo::RefPtr<Cairo::Context>& context, double target_width, double target_height, double x,
        double y, double factor)
{
    update_size(height, target_height, factor);
    update_size(width, target_width, factor);
    context->rectangle(x - (width - target_width)/2, y - (height - target_height)/2, width, height);
}

void ElasticRect::stroke(Cairo::RefPtr<Cairo::Context>& context) const
{
    context->set_source_rgb(0.2, 0.5, 0.3);
    context->set_line_width(7);
    if (width > 5)
        context->stroke();
    context->set_source_rgb(0.1, 0.1, 0.1);
}

void ElasticRect::update_size(double& size, double target_size, double factor)
{
    auto margin = (target_size != 0) ? 20 : 0;
    size += (target_size + margin - size)*factor;
}

void ElasticRect::fill_rect(Cairo::RefPtr<Cairo::Context>& context) const
{
context->set_source_rgb(0.2, 0.2, 0.2);
context->fill();
}
