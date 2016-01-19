///
// statistics.hpp
//
// Provides useful classes for dealing with updating statistics in an game.

#pragma once

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
        finesse(0) {}

    // How many blocks have been placed this game
    int blocks_placed;

    // How many lines cleared this game
    int lines_cleared;

    // The current finesse this game
    int finesse;
}

///
// Specifies a statistic object which tracks progress over an entire game.
struct statistics : public frame_statistics
{
    // Add a frame statistics object to the running total
    void update(const frame_statistics &fstat)
    {
        blocks_placed += fstat.blocks_placed;
        lines_cleared += fstat.lines_cleared;
    }
}

} // namespace mpe
