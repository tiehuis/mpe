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

#include <ncursesw/cursesw.h>

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
    { KEY_C, mpe::keycode::x }
}};

#define BLOCK_FILL "\u25a0 "
#define BLOCK_LINE "\u25a1 "

class ui
{
  public:
    // RenderWindow has no copy constructor
    ui() : window(stdscr)
    {
        window.useColors();
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
        render_field(0, engine);
        window.refresh();
    }

    // Draw the main field
    void render_field(const int ix, const mpe::engine &engine)
    {
        for (int y = 0; y < engine.field.height; ++y) {
            for (int x = 0; x < engine.field.width; ++x) {
                // Adjusted coord variables for drawing
                const int xa = ix + x;
                const int ya = window.height() - 23 - y;

                if (engine.field.at(x, y) || engine.block.at(x, y)) {
                    window.addstr(ya, xa, BLOCK_FILL);
                }
                else if (engine.ghost.at(x, y)) {
                    window.addstr(ya, xa, BLOCK_LINE);
                }
                else {
                    window.addch(ya, xa, ' ');
                }
            }
        }
    }

    void render_preview(const mpe::engine &engine)
    {
    }

    void render_hold(const mpe::engine &engine) {}

    void render_statistics(const mpe::engine &engine)
    {
    }

    ///----------------
    // Member Variables
    ///---

    NCursesColorWindow window;

    Keyboard keyboard;
};

} // namespace mpe
