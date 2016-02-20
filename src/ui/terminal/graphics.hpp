///
// graphics.hpp
//
// Specifies an example ui implementation. This is generic and does not do any
// special drawing based on the mode. This design needs to be revised and
// thought about before any implementation.
//
// It is likely that rules themselves will specify what information to draw,
// but this information should be left to the ui to display. If this could be
// down in a rule-agnostic way that would be ideal.

#pragma once

#include <ui/terminal/linux_input.hpp>
#include <mpe/engine.hpp>

// This class can only have one available instance at any one time active.
// See the member 'instance' for a small discussion on why.
class graphics
{
  public:
    // Initialize a graphical context.
    graphics(bool unicode=true);

    // Destory a graphical context.
    ~graphics();

    // Update the engine with any events read y the graphics context.
    // Currently this is primarily concerned with the updating of keyboard
    // events, but this could be improved in the future.
    void update(mpe::engine &engine);

    // Render the state of the engine onto the screen.
    void render(const mpe::engine &engine) const;

  private:
    // Handle SIGWINCH calls on the current class.
    void sigwinch_handler(int signal);

    // A static signal handler class is required as we cannot use a class
    // method for a signal handler.
    static void static_sigwinch_handler(int signal);

    // Render the specified field state at the specified initial x and y position.
    void render_field(const int ix, const int iy, const mpe::engine &engine) const;

    // Render the specified block at the x and y position.
    void render_block(const int ix, const int iy, const mpe::block &block) const;

    // Render the specified statistics at the x and y position.
    void render_statistics(const int ix, const int iy, const mpe::engine &engine) const;

    // Render the specified preview piece at the x and y position.
    void render_preview(const int ix, const int iy, const mpe::engine &engine) const;

    // Render the specified hold piece at the x and y position.
    void render_hold(const int ix, const int iy, const mpe::engine &engine) const;

    // The current height of the terminal
    int height;

    // The current width of the terminal
    int width;

    // Should the render phase attempt to draw with colour
    bool render_color;

    // Should we render unicode glyphs, or ascii?
    bool unicode;

    // The current device we are using to read events.
    linux_input input;

#ifndef NO_X11
    // The X11 window-id of the terminal which is running this program.
    Window window;

    // The display that the given window exists on.
    Display *display;
#endif

    // This specifies the current instance. This in effect limits the number of
    // available instances of this class to one.
    //
    // This likely isn't as big a problem as one would expect, since ncurses
    // itself is used in a way which does not allow multiple windows to be run
    // at one.
    //
    // This is an effect of not being able to bind extra arguments onto the
    // c style signal handlers.
    static graphics *instance;
};
