///
// statistics.hpp
//
// Provides useful classes for dealing with updating statistics in an game.

#pragma once

#include <cstdio>

namespace mpe {

///
// Specifies a statistic object which tracks the progress over a single frame.
// Private inheritance only inherits the member variables of statistics, which
// is exactly what is required.
struct frame_statistics
{
    frame_statistics() :
        blocks_placed(0),
        lines_cleared(0),
        tspin_count(0)
    {}

    // How many blocks have been placed this frame
    int blocks_placed;

    // How many lines cleared this frame
    int lines_cleared;

    // How many t-spins occurred this frame
    int tspin_count;
};

///
// Specifies a statistic object which tracks progress over an entire game.
struct statistics
{
    statistics() :
        finesse(0),
        blocks_placed(0),
        current_speed(0),
        pps(0),
        ppm(0),
        frames_elapsed(0),
        lines_cleared(0),
        tspin_count(0)
    {}

    // Add a frame statistics object to the running total
    void update(const frame_statistics &fstat)
    {
        blocks_placed += fstat.blocks_placed;
        lines_cleared += fstat.lines_cleared;
        frames_elapsed += 1;
    }

    // Dump the current statistics object to the specified stream
    void dump(FILE *fd = stdout) const
    {
        std::printf("Blocks Placed: %d\n", blocks_placed);
        std::printf("Lines Cleared: %d\n", lines_cleared);

        const float time_elapsed = frames_elapsed * 16.66f / 1000;
        std::printf("Time: %.4fs\n", time_elapsed);
        std::printf("PPS: %.4fs\n", blocks_placed / time_elapsed);
    }

    // The current finesse this game
    int finesse;

    // How many blocks have been placed this frame
    int blocks_placed;

    // Current speed (window)
    double current_speed;

    // Current pieces per second
    double pps;

    // Current pieces per minute
    double ppm;

    // Frames elapsed over entire game
    int frames_elapsed;

    // Total lines cleared this game
    int lines_cleared;

    // Total t-spins this game
    int tspin_count;
};

} // namespace mpe
