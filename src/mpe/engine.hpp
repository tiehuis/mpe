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
#include "mpe/randomizer/interface.hpp"
#include "mpe/wallkick/interface.hpp"
#include "mpe/statistics.hpp"

class engine {
  public:
    ///----------------
    // Member Functions
    ///---

    // Initialize engine with the specified options (hardcoded now)
    engine() : running(true), ticks(0) {
        rule       = std::make_unique<mpe::rule::line_race>();
        randomizer = std::make_unique<mpe::randomizer::bag>();
        wallkick   = std::make_unique<mpe::randomizer::SRS>();
        block      = randomizer->next();
        option     = mpe::option();
    }

    // Perform an update cycle based on the current keystate
    // It would be nice if this was abstracted away slightly.
    void update() {
        // Record all the details which occur in a frame so that these can be
        // passed to any rules
        mpe::frame_statistics stat;

        if (keystate.is_pressed_with_das(keystate::left, option.das))
            block.move_left(field);
        else if (keystate.is_pressed_with_das(keystate::right, option.das))
            block.move_right(field);

        if (keystate.is_pressed_with_das(keystate::down, option.das))
            block.move_down(field);

        if (keystate.is_pushed(keystate::z))
            block.rotate_left(field, *wallkick);
        else if (keystate.is_push(keystate::x))
            block.rotate_right(field, *wallkick);

        if (keystate.is_pressed(keystate::c) && block.can_be_held) {
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

        if (keystate.is_pushed(keystate::space)) {
            block.hard_drop(field);
            field.place_block(block);
            fstat.lines_cleared += field.line_clear();
            block = randomizer->next();
        }

        if (keystate.is_pushed(keystate::q) || rule.end_condition()) {
            running = false;
        }

        rule.update(fstat);
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

    // The current hold block
    std::optional<mpe::block> hold;

    // Options that were specified when starting this game
    mpe::option option;

    // The randomizer style used for this game
    std::unique_ptr<mpe::randomizer::interface> randomizer;

    // The wallkick system used for this game
    std::unique_ptr<mpe::wallkick::interface> wallkick;

    // What type of mode is the current game?
    mpe::mode mode;
};
