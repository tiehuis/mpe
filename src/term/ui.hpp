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

#include <signal.h>

#include <ncurses.h>

#include <mpe/engine.hpp>
#include <mpe/keystate.hpp>
#include <mpe/utility.hpp>

#include "keyboard.hpp"

namespace mpe {

struct Key {
    int key;
    mpe::keycode index;
};

static constexpr std::array<Key, mpe::keycode_length> keymap = {{
    { KEY_LEFT, mpe::keycode::left },
    { KEY_RIGHT, mpe::keycode::right },
    { KEY_DOWN, mpe::keycode::down },
    { KEY_UP, mpe::keycode::up },
    { KEY_SPACE, mpe::keycode::space },
    { KEY_Z, mpe::keycode::z },
    { KEY_X, mpe::keycode::x },
    { KEY_Q, mpe::keycode::q },
    { KEY_C, mpe::keycode::c }
}};

class ui
{
  public:
    // RenderWindow has no copy constructor
    ui()
    {
        initscr();
        curs_set(0);
        timeout(0);
        noecho();
        if (has_colors())
            start_color();

        // Initialize all the colours
        init_color(bI,    0, 1000, 1000);
        init_color(bT,    0,    0, 1000);
        init_color(bL, 1000,  647,    0);
        init_color(bJ, 1000, 1000,    0);
        init_color(bS,    0, 1000,    0);
        init_color(bZ,  667,    0, 1000);
        init_color(bO, 1000,    0,    0);

        // Initialize all color pairs
        init_pair(1, bI, 0);
        init_pair(2, bT, 0);
        init_pair(3, bL, 0);
        init_pair(4, bJ, 0);
        init_pair(5, bS, 0);
        init_pair(6, bZ, 0);
        init_pair(7, bI, 0); // White blocks are not rendered properly by ncurses

        // Get size of screen
        getmaxyx(stdscr, height, width);
    }

    ~ui()
    {
        // Read all buffered input so it isn't dumped on exit
        while (getch() != ERR) {}
        endwin();
    }

    // Update the currently set keys
    void update(mpe::engine &engine)
    {
        keyboard.read_events();
        for (auto key : keymap) {
            if (keyboard.is_set(key.key))
                engine.keystate.key_down(key.index);
            else
                engine.keystate.key_up(key.index);
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

                if (engine.field.at(x, y)) {
                    attron(COLOR_PAIR(engine.field.at(x, y)));
                    mvaddstr(ya, xa, "\u25a0 ");
                    attroff(COLOR_PAIR(engine.field.at(x, y)));
                } else if (engine.block.at(x, y)) {
                    attron(COLOR_PAIR(engine.block.id + 1));
                    mvaddstr(ya, xa, "\u25a0 ");
                    attroff(COLOR_PAIR(engine.block.id + 1));
                }
                else if (engine.ghost.at(x, y)) {
                    attron(COLOR_PAIR(engine.ghost.id + 1));
                    mvaddstr(ya, xa, "\u25a1 ");
                    attroff(COLOR_PAIR(engine.ghost.id + 1));
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
        attron(COLOR_PAIR(block.id + 1));
        for (int i = 0; i < block.data.size(); ++i) {
            const int xa = x + 2 * block.data[i].x;
            const int ya = y - block.data[i].y;
            mvaddstr(ya, xa, "\u25a0 ");
        }
        attroff(COLOR_PAIR(block.id + 1));
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

        mvprintw(ya + 0, ix, "Blocks Placed: %d", engine.statistics.blocks_placed);

        mvprintw(ya + 2, ix, "Lines Cleared: %d", engine.statistics.lines_cleared);

        const float time_elapsed = engine.statistics.frames_elapsed * 16.66f / 1000;
        mvprintw(ya + 4, ix, "Time: %.4fs", time_elapsed);

        const float pps = engine.statistics.blocks_placed / time_elapsed;
        mvprintw(ya + 6, ix, "PPS: %.4fs", pps);
    }

    ///----------------
    // Member Variables
    ///---

    int height;

    int width;

    Keyboard keyboard;
};

} // namespace mpe
