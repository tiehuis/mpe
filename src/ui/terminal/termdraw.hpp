///
// termdraw.hpp
//
// A simplistic C++ ncurses-like file for easier drawing to a terminal. This is
// not intended to match the scope of ncurses, and many comparable features
// will be missing. Portability is limited to terminals which support vt100
// escape codes.
//
// We implement a simple double-buffering system and the ability to draw onto a
// single terminal instance.

#include <stdint.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <clocale>

// Copyright (c) 2008-2009 Bjoern Hoehrmann <bjoern@hoehrmann.de>
// See http://bjoern.hoehrmann.de/utf-8/decoder/dfa/ for details.

#define UTF8_ACCEPT 0
#define UTF8_REJECT 1

namespace {
    static const uint8_t utf8d[] = {
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 00..1f
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 20..3f
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 40..5f
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 60..7f
      1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9, // 80..9f
      7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7, // a0..bf
      8,8,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, // c0..df
      0xa,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x4,0x3,0x3, // e0..ef
      0xb,0x6,0x6,0x6,0x5,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8, // f0..ff
      0x0,0x1,0x2,0x3,0x5,0x8,0x7,0x1,0x1,0x1,0x4,0x6,0x1,0x1,0x1,0x1, // s0..s0
      1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,0,1,1,1,1,1,1, // s1..s2
      1,2,1,1,1,1,1,2,1,2,1,1,1,1,1,1,1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1, // s3..s4
      1,2,1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1,1,1,1,1,1,3,1,3,1,1,1,1,1,1, // s5..s6
      1,3,1,1,1,1,1,3,1,3,1,1,1,1,1,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1, // s7..s8
    };

    uint32_t inline decode(uint32_t* state, uint32_t* codep, uint32_t byte) {
        uint32_t type = utf8d[byte];

        *codep = (*state != UTF8_ACCEPT) ?
            (byte & 0x3fu) | (*codep << 6) :
            (0xff >> type) & (byte);

        *state = utf8d[256 + *state*16 + type];
        return *state;
    }

} // anonymous namespace

namespace termdraw {

///
// A window is the current terminal window context. By default this is made as
// large as possible.
class window {

    window() : cursor(0)
    {
        // Adjust the initial locale
        std::setlocale(LC_ALL, "");

        _retrieve_window_dimensions();

        // Setup signal handler for terminal resize
        signal(SIGWINCH, _handle_window_resize);
    }

    void _handle_window_resize(int signal)
    {
        // Disable signal handler during processing
        signal(SIGWINCH, SIG_IGN);

        _retrieve_window_dimensions();

        // Restore signal handler
        signal(SIGWINCH, _handle_window_resize);
    }

    void _retrieve_window_dimensions()
    {
        struct winsize ws;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
        height = ws.ws_row;
        width  = ws.ws_col;
        back.resize(width * height);
        front.resize(width * height);
    }

    // Insert a codepoint into the backbuffer and update the cursor, wrapping
    // back to the start if required.
    void _insert_into_backbuffer(size_t &cursor, const uint32_t codepoint)
    {
        if (cursor >= backbuffer.size())
            cursor = 0;

        backbuffer[cursor++] = codepoint;
    }

    // Clear the entire screen. This occurs immediately and does not require an
    // update call.
    void clear(void)
    {
        std::puts("\e[2J\e[H");
        std::fill(buffers->back().begin(), buffers->back().end(), 0);
    }

    // Save the entire screen contents.
    void save_and_clear(void)
    {
        std::puts("\e7\e[?47h");
        std::fill(buffers->back().begin(), buffers->back().end(), 0);
    }

    // Restore a saved screen
    void restore(void)
    {
        std::puts("\e[?47l");
    }

    // Decode the given string as as utf-8 and insert into the backbuffer. If
    // the string is invalid utf-8, then we discard any remaining values and
    // return false.
    bool puts(const char *str)
    {
        uint32_t codepoint;
        uint32_t state = 0;

        for (; *str; ++str) {
            switch (decode(&state, &codepoint, *str)) {
              case UTF8_ACCEPT:
                _insert_into_backbuffer(cursor, codepoint);
                break;
              case UTF8_REJECT:
                return false;
            }
        }

        return true;
    }

    bool mvputs(const int x, const int y, const char *str)
    {
        cursor = y * width + x;
        return puts(str);
    }

    template <typename... Ts>
    bool printf(const char *fmt, Ts... ts)
    {
        // Save the current width in case of signal mid-function
        const size_t width_point = width;

        // Assume that one would never want to print longer than a terminal row
        char buf[width_point];
        std::snprintf(buffer, width_point, fmt, ts...);

        return puts(buf);
    }

    template <typename... Ts>
    bool mvprintf(const int x, const int y, const char *fmt, Ts... ts)
    {
        cursor = y * width + x;
        return printf(fmt, ts...);
    }

    // Perform the actual writing out and swapping of buffers
    void update()
    {
        auto back = buffers->back();
        auto frnt = buffers->front();

        // Iterate over the backbuffer, writing any changes as we come to them
        for (int i = 0; i < back.size(); ++i) {
            if (back[i] != front[i]) {
                // Write out this codepoint
                const int x = i % width;
                const int y = i / width;

                // Move to absolute position
                std::printf("\e12414");
                std::printf("%c", codepoint);

                front[i] = back[i];
            }
        }
    }

    ///
    // Variables

    // The height of the terminal window in characters
    int height;

    // The width of the terminal window in characters
    int width;

    // Current terminal cursor position
    size_t cursor;

    // Backbuffer storing current write state
    std::vector<uint32_t> back;

    // Frontbuffer storing current screen state
    std::vector<uint32_t> front;
};

} // namespace termdraw

int main(void)
{
    termdraw::window window;
    window.save();



    window.restore();
}
