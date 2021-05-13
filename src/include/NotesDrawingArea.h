#ifndef KBI_NOTESDRAWINGAREA_H
#define KBI_NOTESDRAWINGAREA_H

#include <gtkmm/drawingarea.h>
#include "Player.h"

class NotesDrawingArea : public Gtk::DrawingArea {
public:
    explicit NotesDrawingArea(Player* p_player);

    ~NotesDrawingArea() override;

private:
    Pango::FontDescription font;

    void on_draw(const Cairo::RefPtr<Cairo::Context>& cr, int width, int height);

    static void draw_rectangle(const Cairo::RefPtr<Cairo::Context>& cr, int width, int height);

    void draw_text(const Cairo::RefPtr<Cairo::Context>& cr, int rectangle_width, int rectangle_height);

    Player* player;

    bool refresh();

    static string get_span_tag(const string& text, const string& color);

    static string get_tagged(const string& text, const string& tag);

    shared_ptr<Pango::Layout> layout;
};

#endif //KBI_NOTESDRAWINGAREA_H
