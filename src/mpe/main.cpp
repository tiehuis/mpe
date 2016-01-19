#include <chrono>
#include <ratio>
#include <thread>

#include "mpe/engine.hpp"
#include "mpe/ui.hpp"

// Frame rate specifies the number of frame updates per second
constexpr int framerate = 60;

// Tick rate specifies the number of engine updates per second. Tickrates
// should be a multiple of the framerate to mean anything.
constexpr int tickrate = 2 * framerate;

// It makes no sense for tickrate to be less than framerate
static_assert(tickrate > framerate);

// Define the duration of a single tick
constexpr std::chrono::duration<int, std::ratio<1, framerate>> time(1);

int main(void)
{
    mpe::engine engine;
    mpe::ui ui(framerate);

    while (engine.running) {
        auto next_time_point = std::chrono::steady_clock::now() + frametime;

        // Limit draw phase to 60 frames regardless of internal tick counter
        if (engine.ticks % (tickrate / framerate)) {
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
