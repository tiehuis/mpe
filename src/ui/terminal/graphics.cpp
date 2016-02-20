///
// graphics.cpp
//
// Implementation of a ui based on ncurses.
// Specifies an example ui implementation. This is generic and does not do any
// special drawing based on the mode. This design needs to be revised and
// thought about before any implementation.
//
// It is likely that rules themselves will specify what information to draw,
// but this information should be left to the ui to display. If this could be
// down in a rule-agnostic way that would be ideal.

#include <tuple>
#include <utility>
#include <ncurses.h>
#include <signal.h>

#include <mpe/engine.hpp>
#include <ui/terminal/graphics.hpp>
#ifndef NO_X11
#include <ui/terminal/x11_window.hpp>
#endif

graphics *graphics::instance = nullptr;

// Initialize of ncurses colour pallete
static void initialize_colors(bool reverse_colorscheme = false)
{
    init_color(bI,    0, 1000, 1000);
    init_color(bT,    0,    0, 1000);
    init_color(bL, 1000,  647,    0);
    init_color(bJ, 1000, 1000,    0);
    init_color(bS,    0, 1000,    0);
    init_color(bZ,  667,    0, 1000);
    init_color(bO, 1000,    0,    0);

    if (!reverse_colorscheme) {
        init_pair(1, bI, 0);
        init_pair(2, bT, 0);
        init_pair(3, bL, 0);
        init_pair(4, bJ, 0);
        init_pair(5, bS, 0);
        init_pair(6, bZ, 0);
        // TODO: White blocks are not rendered well by lxterminal
        init_pair(7, bO, 0);
    }
    else {
        init_pair(1, 0, bI);
        init_pair(2, 0, bT);
        init_pair(3, 0, bL);
        init_pair(4, 0, bJ);
        init_pair(5, 0, bS);
        init_pair(6, 0, bZ);
        init_pair(7, 0, bO);
    }
}

graphics::graphics(const bool unicode_) :
    render_color(false), unicode(unicode_)
{
    // Attempt to replace signal handler action
    // If we set this before ncurses, this will be called along with the
    // ncurses signal handler.
    const struct sigaction new_sigaction = {
        // sa_sigaction and sa_handler may be a union
        .sa_handler = static_sigwinch_handler,
        .sa_flags   = SA_NODEFER
    };

    sigaction(SIGWINCH, &new_sigaction, nullptr);
    graphics::instance = this;

    initscr();
    curs_set(false);
    timeout(false);
    noecho();           // Required to avoid end-game character dump

    if (has_colors()) {
        render_color = true;
        start_color();
        initialize_colors(unicode ? false : true);
    }

    // Macro: height and width are modified.
    getmaxyx(stdscr, height, width);

#ifndef NO_X11
    // Attempt to utilize the X11 input system to avoid requiring root
    // permissions, falling back to the linux/input.h method otherwise.
    display = nullptr;
    window = 0;
#endif
}

graphics::~graphics()
{
    // Read all buffered input so it isn't dumped on exit
    while (getch() != ERR) {}
    endwin();
}

void graphics::static_sigwinch_handler(int signo)
{
    graphics::instance->sigwinch_handler(signo);
}

void graphics::sigwinch_handler(int signo)
{
    // This recalculates the internal cols and lines variables
    endwin();
    refresh();

    // Macro: height and width are modified.
    getmaxyx(stdscr, height, width);
    erase();
}

void graphics::update(mpe::engine &engine)
{
    using keycode_map = std::pair<int, mpe::keycode>;

    static const std::array<keycode_map, mpe::keycode_length> keymap = {{
        { KEY_LEFT  , mpe::keycode::left },
        { KEY_RIGHT , mpe::keycode::right },
        { KEY_DOWN  , mpe::keycode::down },
        { KEY_UP    , mpe::keycode::up },
        { KEY_SPACE , mpe::keycode::space },
        { KEY_Z     , mpe::keycode::z },
        { KEY_X     , mpe::keycode::x },
        { KEY_Q     , mpe::keycode::q },
        { KEY_C     , mpe::keycode::c }
    }};

    input.read_events();
    for (auto key : keymap) {
        if (input.is_down(key.first)) {
            engine.keystate.key_down(key.second);
        }
        else
            engine.keystate.key_up(key.second);
    }
}

void graphics::render(const mpe::engine &engine) const
{
    erase();

    // WHere the upper region of the draw window should be located
    const int iy = (height - 26) / 2;

    // 10 blocks + 2 borders
    const int field_width = 2 * 10 + 2;

    // 4 blocks
    const int preview_width = 2 * 4;

    // 4 blocks
    const int hold_width = 2 * 4;

    // 20 characters wide
    const int statistics_width = 24;

    // 2 characters wide
    const int seperator_width = 2;

    // Cannot fit the field into this viewport.
    // TODO: Should we really abort? Or just crop the output?
    if (width < field_width) {
        std::exit(1);
    }
    // Field only.
    else if (width < field_width + preview_width) {
        const int xoffset = (field_width + preview_width - width) / 2;
        render_field(xoffset, iy, engine);
    }
    // Field, Preview.
    else if (width < field_width + preview_width + hold_width +
            seperator_width) {
        const int xoffset = (field_width + preview_width - width) / 2;
        render_field(xoffset, iy, engine);
        render_preview(xoffset + field_width + seperator_width, iy, engine);
    }
    // Field, Preview, Hold
    else if (width < field_width + preview_width + hold_width +
            statistics_width + 2 * seperator_width) {
        const int xoffset = (field_width + preview_width + hold_width
                - width) / 2;
        render_hold(xoffset, iy, engine);
        render_field(xoffset + hold_width + seperator_width, iy, engine);
        render_preview(xoffset + seperator_width + hold_width +
                seperator_width + field_width, iy, engine);
    }
    // Field, Preview, Hold, Statistics
    else {
        const int xoffset = (width - field_width - preview_width - hold_width -
                statistics_width) / 2;
        render_hold(xoffset, iy, engine);
        render_field(xoffset + hold_width + seperator_width, iy, engine);
        render_preview(xoffset + hold_width + seperator_width +
                field_width + seperator_width, iy, engine);
        render_statistics(xoffset + hold_width + seperator_width +
                field_width + seperator_width + preview_width +
                seperator_width, iy, engine);
    }

    refresh();
}

void graphics::render_field(const int ix, const int iy,
        const mpe::engine &engine) const
{
    for (int y = 0; y < engine.field.height; ++y) {
        const int ya = iy + engine.field.height - y;
        attron(unicode ? A_NORMAL : A_REVERSE);
        mvaddstr(ya, ix, unicode ? "\u2502" : " ");
        attroff(unicode ? A_NORMAL : A_REVERSE);

        for (int x = 0; x < engine.field.width; ++x) {
            const int xa = 1 + ix + 2 * x;

            if (engine.field.at(x, y)) {
                attron(COLOR_PAIR(engine.field.at(x, y)));
                mvaddstr(ya, xa, unicode ? "\u25a0 " : "  ");
                attroff(COLOR_PAIR(engine.field.at(x, y)));
            } else if (engine.block.at(x, y)) {
                attron(COLOR_PAIR(engine.block.id + 1));
                mvaddstr(ya, xa, unicode ? "\u25a0 " : "  ");
                attroff(COLOR_PAIR(engine.block.id + 1));
            }
            else if (engine.ghost.at(x, y)) {
                attron(COLOR_PAIR(engine.ghost.id + 1));
                mvaddstr(ya, xa, unicode ? "\u25a1 " : "  ");
                attroff(COLOR_PAIR(engine.ghost.id + 1));
            }
        }

        attron(unicode ? A_NORMAL : A_REVERSE);
        mvaddstr(ya, 1 + ix + 2 * engine.field.width, unicode ? "\u2502" : " ");
        attroff(unicode ? A_NORMAL : A_REVERSE);
    }

    attron(unicode ? A_NORMAL : A_REVERSE);
    mvaddstr(iy + engine.field.height + 1, ix, unicode ? "\u2514" : " ");
    attroff(unicode ? A_NORMAL : A_REVERSE);

    for (int x = 0; x < engine.field.width; ++x)
        addstr(unicode ? "\u2500\u2500" : "__");

    attron(unicode ? A_NORMAL : A_REVERSE);
    addstr(unicode ? "\u2518" : " ");
    attroff(unicode ? A_NORMAL : A_REVERSE);
}


void graphics::render_block(const int x, const int y,
        const mpe::block &block) const
{
    attron(COLOR_PAIR(block.id + 1));
    for (int i = 0; i < block.data.size(); ++i) {
        const int xa = x + 2 * block.data[i].x;
        const int ya = y - block.data[i].y;
        mvaddstr(ya, xa, unicode ? "\u25a0 " : "  ");
    }
    attroff(COLOR_PAIR(block.id + 1));
}

void graphics::render_preview(const int ix, const int iy,
        const mpe::engine &engine) const
{
    if (engine.randomizer->preview_count() == 0)
        return;

    auto pieces = engine.randomizer->preview_pieces();

    for (int i = 0; i < std::min(engine.randomizer->preview_count(), 4); ++i) {
        mpe::block preview(pieces[i]);
        render_block(ix, iy + i * 5, preview);
    }
}

void graphics::render_hold(const int ix, const int iy,
        const mpe::engine &engine) const
{
    if (!engine.hold)
        return;

    mpe::block hold(engine.hold.value().id);
    render_block(ix, iy, hold);
}

void graphics::render_statistics(const int ix, const int iy,
        const mpe::engine &engine) const
{
    const int ya = iy + 4;

    mvprintw(ya + 0, ix, "Blocks Placed: %d", engine.statistics.blocks_placed);

    mvprintw(ya + 2, ix, "Lines Cleared: %d", engine.statistics.lines_cleared);

    const float time_elapsed = engine.statistics.frames_elapsed * 16.66f / 1000;
    mvprintw(ya + 4, ix, "Time: %.4fs", time_elapsed);

    const float pps = engine.statistics.blocks_placed / time_elapsed;
    mvprintw(ya + 6, ix, "PPS: %.4fs", pps);
}
