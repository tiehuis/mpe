#include <chrono>
#include <ratio>
#include <thread>

#include <clocale>
#include <mpe/engine.hpp>

#include "ui.hpp"

// Frame rate specifies the number of frame updates per second
constexpr int framerate = 60;

// Tick rate specifies the number of engine updates per second. Tickrates
// should be a multiple of the framerate to mean anything.
constexpr int tickrate = framerate;

// It makes no sense for tickrate to be less than framerate
static_assert(tickrate >= framerate);

// Define the duration of a single tick
constexpr std::chrono::duration<int, std::ratio<1, tickrate>> ticktime(1);

int main(void)
{
    // Important to call this before the ui constructor
    std::setlocale(LC_ALL, "");

    mpe::engine engine;
    mpe::ui ui;

    while (engine.running) {
        auto next_time_point = std::chrono::steady_clock::now() + ticktime;

        // Limit draw phase to 60 frames regardless of internal tick counter
        if (engine.ticks % (tickrate / framerate) == 0) {
            // Render current game status
            ui.render(engine);

            // Update engine keys
            ui.update(engine);
        }

        // Process game tick
        engine.update();

        std::this_thread::sleep_until(next_time_point);
    }
}
