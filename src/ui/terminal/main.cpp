///
// main.cpp
//
// The frontend entry point for the terminal interface.

#include <chrono>
#include <clocale>
#include <ratio>
#include <thread>

#include <mpe/engine.hpp>
#include <ui/terminal/graphics.hpp>

constexpr int framerate = 60;

// Tick rate specifies the number of engine updates per second. Tickrates
// should be a multiple of the framerate to mean anything.
constexpr int tickrate = framerate;

// It makes no sense for tickrate to be less than framerate
static_assert(tickrate >= framerate);

constexpr std::chrono::duration<int, std::ratio<1, tickrate>> ticktime(1);

int main(int argc, char **argv)
{
    // Ensure we aren't using the C/Ascii locale so unicode characters render
    std::setlocale(LC_ALL, "");
    mpe::engine engine;

    bool unicode = argc > 1 ? false : true;

    // Close the graphics class seperately so that we can perform actions
    // once the graphics context has been cleared.
    {
        graphics gfx(unicode);

        while (engine.running) {
            auto next_time_point = std::chrono::steady_clock::now() + ticktime;

            // Limit draw phase to 60 frames regardless of internal tick counter
            if (engine.ticks % (tickrate / framerate) == 0) {
                gfx.render(engine);
                gfx.update(engine);
            }

            engine.update();

            // TODO: This assumes that we will complete processing in one frame
            std::this_thread::sleep_until(next_time_point);
        }
    }

    engine.statistics.dump();
}
