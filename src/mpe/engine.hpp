///
// engine.hpp
//
// Combines all the individual mpe components into a superclass which declares
// a high-level API for interacting with these components.

#pragma once

#include <algorithm>
#include <memory>
#include <thread>
#include <experimental/optional>

#include "mpe/block.hpp"
#include "mpe/field.hpp"
#include "mpe/keystate.hpp"
#include "mpe/option.hpp"
#include "mpe/randomizer/bag.hpp"
#include "mpe/wallkick/srs.hpp"
#include "mpe/rule/line_race.hpp"
#include "mpe/statistics.hpp"

#include <stdio.h>

namespace mpe {

class engine {
  public:
    ///----------------
    // Member Functions
    ///---

    // Initialize engine with the specified options (hardcoded now)
    engine() : running(true), ticks(0) {
        rule       = std::make_unique<mpe::rule::line_race>();
        randomizer = std::make_unique<mpe::randomizer::bag>();
        wallkick   = std::make_unique<mpe::wallkick::SRS>();
        block      = mpe::block(randomizer->next());
        option     = mpe::option();
    }

    void update_move() {
        // Move in the direction that has been pressed the most recently. This
        // is much more natural behaviour when we have low DAS values.
        if (keystate.times[keycode::left] && keystate.times[keycode::right]) {
            if (keystate.times[keycode::left] < keystate.times[keycode::right]) {
                if (keystate.is_pressed_with_das(keycode::left, option.das)) {
                    block.move_left(field);
                }
            }
            else {
                if (keystate.is_pressed_with_das(keycode::right, option.das)) {
                    block.move_right(field);
                }
            }
        }
        else if (keystate.times[keycode::left]) {
            if (keystate.is_pressed_with_das(keycode::left, option.das)) {
                block.move_left(field);
            }
        }
        else if (keystate.times[keycode::right]) {
            if (keystate.is_pressed_with_das(keycode::right, option.das)) {
                block.move_right(field);
            }
        }

        if (keystate.is_pressed_with_das(keycode::down, option.das))
            block.move_down(field);

        if (keystate.is_pushed(keycode::z))
            block.rotate_left(field, *wallkick);
        else if (keystate.is_pushed(keycode::x))
            block.rotate_right(field, *wallkick);
    }

    // Perform an update cycle based on the current keystate
    // It would be nice if this was abstracted away slightly.
    void update() {
        // Record all the details which occur in a frame so that these can be
        // passed to any rules
        mpe::frame_statistics fstat;

        // While we know the current keystate, we need to update the timings
        keystate.update_all();

        // We may need to perform a move multiple times in one frame, depending
        // on the current DAS state.
        update_move();

        if (keystate.is_pressed(keycode::c) && block.can_be_held) {
            if (!hold) {
                hold = mpe::block(block.id);
                block = randomizer->next();
            }
            else {
                // Reset block position before holding it
                block = mpe::block(block.id);
                std::swap(hold.value(), block);
            }
            block.can_be_held = false;
        }

        if (keystate.is_pushed(keycode::space)) {
            block.hard_drop(field);
            field.place_block(block);
            fstat.lines_cleared += field.line_clear();
            block = randomizer->next();
        }

        ghost = block;
        ghost.hard_drop(field);

        if (keystate.is_pushed(keycode::q) || rule->end_condition()) {
            running = false;
        }

        rule->update(fstat);
        statistics.update(fstat);

        // This update loop is called every tick. We should take into account
        // the possibility that an update is skipped, what should happen to
        // this tick count in that case?
        ticks++;
    }

    ///----------------
    // Member Variables
    ///---

    // Is the game still running?
    bool running;

    // How many ticks have elapsed since this game started?
    int ticks;

    // The current rule we are playing with
    std::unique_ptr<mpe::rule::interface> rule;

    // The state of the system key peripherals
    mpe::keystate keystate;

    // The state of the field
    mpe::field field;

    // The current block in play
    mpe::block block;

    // Ghost piece
    mpe::block ghost;

    // Statistics for the current game
    mpe::statistics statistics;

    // The current hold block
    std::experimental::optional<mpe::block> hold;

    // Options that were specified when starting this game
    mpe::option option;

    // The randomizer style used for this game
    std::unique_ptr<mpe::randomizer::interface> randomizer;

    // The wallkick system used for this game
    std::unique_ptr<mpe::wallkick::interface> wallkick;

    // What type of mode is the current game?
    std::unique_ptr<mpe::rule::interface> mode;
};

} // namspace mpe
