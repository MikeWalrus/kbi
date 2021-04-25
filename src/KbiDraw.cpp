//
// Created by 86176 on 2021/4/25.
//

#include "KbiDraw.h"
#include <cairomm/context.h>

KbiDraw::KbiDraw() {
    set_content_width(300);
    set_content_height(300);
    set_expand(true);
    set_draw_func(sigc::mem_fun(*this, &KbiDraw::on_draw));

}

KbiDraw::~KbiDraw() {

}

void KbiDraw::on_draw(const Cairo::RefPtr<Cairo::Context> &cr,int width, int height) {

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

void KbiDraw::draw_rectangle(const Cairo::RefPtr<Cairo::Context> &cr, int width, int height) {

    cr->rectangle(0, 0, width, height);
    cr->fill();

}

void KbiDraw::draw_text(const Cairo::RefPtr<Cairo::Context> &cr, int rectangle_width, int rectangle_height) {

    Pango::FontDescription font;

    font.set_family("Monospace");
    font.set_weight(Pango::Weight::BOLD);

    font.set_size(35000);
    auto layout = create_pango_layout("Kbi Player");

    layout->set_font_description(font);

    int text_width;
    int text_height;

    //get the text dimensions (it updates the variables -- by reference)
    layout->get_pixel_size(text_width, text_height);

    // Position the text in the middle
    cr->move_to((rectangle_width-text_width)/2, (rectangle_height-text_height)/2);

    layout->show_in_cairo_context(cr);

}