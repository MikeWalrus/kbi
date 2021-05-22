#ifndef KBI_NOTESDRAWINGAREA_H
#define KBI_NOTESDRAWINGAREA_H

#include <gtkmm/drawingarea.h>
#include "Player.h"

class ElasticRect {
public:
    void
    draw_border(Cairo::RefPtr<Cairo::Context> context, double target_width, double target_height, double x, double y,
            double factor = 0.5);

    void
    draw_rect_fill(Cairo::RefPtr<Cairo::Context> context, double target_width, double target_height, double x, double y,
            double factor);

private:
    double height{}, width{};

    static void update_size(double& size, double target_size, double factor);

    void stroke(Cairo::RefPtr<Cairo::Context>& context) const;

    static void fill_rect(Cairo::RefPtr<Cairo::Context>& context);

    void
    update_rect(Cairo::RefPtr<Cairo::Context>& context, double target_width, double target_height, double x, double y,
            double factor);
};

class NotesDrawingArea : public Gtk::DrawingArea {
public:
    explicit NotesDrawingArea(Player* p_player);

    ~NotesDrawingArea() override;

private:
    Pango::FontDescription font;

    void on_draw(const Cairo::RefPtr<Cairo::Context>& cr, int width, int height);

    static void draw_rectangle(const Cairo::RefPtr<Cairo::Context>& cr, int width, int height);

    void draw_text(const shared_ptr<Cairo::Context> cr, int rectangle_width, int rectangle_height);

    bool refresh();

    static string get_span_tag(const string& text, const string& color);

    static string get_tagged(const string& text, const string& tag);

    Player* player;
    shared_ptr<Pango::Layout> layout;
    ElasticRect rect_notes, rect_instrument;

    static constexpr const char* green = "#b3ff70";
    static constexpr const char* blue = "#7ac6ff";
    static constexpr const char* bluish_green = "#42f5b3";
    static constexpr const char* grey = "#444444";

    static void break_line(string& markup_text, int count);

};

#endif //KBI_NOTESDRAWINGAREA_H
