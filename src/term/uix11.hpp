///
// ui.hpp
//
// Specifies an example ui implementation. This is generic and does not do any
// special drawing based on the mode. This design needs to be revised and
// thought about before any implementation.
//
// It is likely that rules themselves will specify what information to draw,
// but this information should be left to the ui to display. If this could be
// down in a rule-agnostic way that would be ideal.

#pragma once

#include <array>
#include <algorithm>

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <signal.h>
#include <ncurses.h>

#include <mpe/engine.hpp>
#include <mpe/keystate.hpp>
#include <mpe/utility.hpp>

// X11 utility functions

namespace mpe {

class ui
{
  public:
    // RenderWindow has no copy constructor
    ui()
    {
        initscr();
        curs_set(0);

        // Get size of screen
        getmaxyx(stdscr, height, width);

        display = XOpenDisplay(0);
        window = getTerminalWindow(display);
        XSelectInput(display, window, KeyPressMask | KeyReleaseMask);
    }

    ~ui()
    {
        endwin();
    }

    // Update the currently set keys
    void update(mpe::engine &engine)
    {
        XEvent e;

        while (XCheckMaskEvent(display, KeyPressMask | KeyReleaseMask, &e)) {
            // If a key is released set it to 0
            // If a key is pressed, set it to 1

            mpe::keycode index;

            switch (XLookupKeysym(&e.xkey, 0)) {
              case XK_Left:
                index = mpe::keycode::left;
                break;
              case XK_Right:
                index = mpe::keycode::right;
                break;
              case XK_Down:
                index = mpe::keycode::down;
                break;
              case XK_Up:
                index = mpe::keycode::up;
                break;
              case XK_space:
                index = mpe::keycode::space;
                break;
              case XK_Z:
                index = mpe::keycode::z;
                break;
              case XK_X:
                index = mpe::keycode::x;
                break;
              case XK_C:
                index = mpe::keycode::c;
                break;
              case XK_Q:
                index = mpe::keycode::q;
                break;
              default:
                // Invalid keycode
                continue;
            }

            switch (e.type) {
              case KeyPress:
                engine.keystate.key_down(index);
                break;
              case KeyRelease:
                engine.keystate.key_up(index);
                break;
              default:
                break;
            }
        }
    }

    ///
    // We render selectively based on the current size of the terminal as
    // follows:
    //
    // Width:
    // 0  - 12 - Fail
    // 12 - 18 - Field only
    // 18 - 25 - Field, Preview
    // 25 - 50 - Field, Preview, Hold
    // 50 - .. - Field, Preview, Hold, Statistics
    //
    // Height:
    // 0  - 22 - Fail
    // 22 - .. - Field, Preview, Hold, Statistics
    void render(const mpe::engine &engine)
    {
        erase();

        if (width < 2 * 12) {
            abort();
        }
        else if (width < 2 * 19) {
            render_field(2 * 1, engine);
            render_preview(2 * 13, engine);
        }
        else if (width < 2 * 24) {
        }
        else {
            render_hold(2 * 1, engine);
            render_field(2 * 6, engine);
            render_preview(2 * 18, engine);
            render_statistics(2 * 25, engine);
        }

        refresh();
    }

    // Draw the main field
    void render_field(const int ix, const mpe::engine &engine)
    {
        for (int y = 0; y < engine.field.height; ++y) {
            const int ya = height / 2 - y;
            mvaddstr(ya, ix, "\u2502");

            for (int x = 0; x < engine.field.width; ++x) {
                const int xa = 1 + ix + 2 * x;

                if (engine.field.at(x, y) || engine.block.at(x, y)) {
                    mvaddstr(ya, xa, "\u25a0 ");
                }
                else if (engine.ghost.at(x, y)) {
                    mvaddstr(ya, xa, "\u25a1 ");
                }
            }

            mvaddstr(ya, 1 + ix + 2 * engine.field.width, "\u2502");
        }

        mvaddstr(height / 2 + 1, ix, "\u2514");
        for (int x = 0; x < engine.field.width; ++x)
            addstr("\u2500\u2500");
        addstr("\u2518");
    }

    void render_block(const int x, const int y, mpe::block &block) const
    {
        for (int i = 0; i < 4; ++i) {
            const int xa = x + 2 * block.data[i].x;
            const int ya = y - block.data[i].y;
            mvaddstr(ya, xa, "\u25a0 ");
        }
    }

    void render_preview(const int ix, const mpe::engine &engine)
    {
        if (engine.randomizer->preview_count() == 0)
            return;

        auto pieces = engine.randomizer->preview_pieces();
        const int iy = height / 2 - engine.field.height;

        for (int i = 0; i < std::min(engine.randomizer->preview_count(), 4); ++i) {
            mpe::block preview(pieces[i]);
            render_block(ix, iy + i * 5, preview);
        }
    }

    void render_hold(const int ix, const mpe::engine &engine)
    {
        if (!engine.hold)
            return;

        mpe::block hold(engine.hold.value().id);
        render_block(ix, height / 2 - engine.field.height, hold);
    }

    void render_statistics(const int ix, const mpe::engine &engine)
    {
        const int ya = height / 2 - engine.field.height + 4;

        mvprintw(ya, ix, "Lines Cleared: %d", engine.statistics.lines_cleared);
    }

    ///----------------
    // Member Variables
    ///---

    int height;

    int width;

    Display *display;

    Window window;
};

} // namespace mpe
