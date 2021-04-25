//
// Created by 86176 on 2021/4/25.
//

#ifndef KBI_KBIDRAW_H
#define KBI_KBIDRAW_H

#include <gtkmm/drawingarea.h>


class KbiDraw : public Gtk::DrawingArea {
public:
    KbiDraw();
    virtual ~KbiDraw();

protected:
    void on_draw(const Cairo::RefPtr<Cairo::Context>& cr, int width, int height);

private:
    void draw_rectangle(const Cairo::RefPtr<Cairo::Context>& cr, int width, int height);
    void draw_text(const Cairo::RefPtr<Cairo::Context>& cr, int rectangle_width, int rectangle_height);

};


#endif //KBI_KBIDRAW_H
