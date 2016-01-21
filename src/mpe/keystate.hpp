///
// keystate.hpp
//
// Specifies the state of a set of restricted inputs.
//
// Improvements:
//  - There are quite a few enum_casts here. It isn't too bad, but it would be
//    nice if there were a cleaner alternative.

#pragma once

#include <algorithm>
#include <array>

#include "mpe/utility.hpp"

namespace mpe {

enum keycode {
    left, right, up, down, z, x, c, q, space,
    keycode_length
};

class keystate
{
  public:
    ///----------------
    // Member Functions
    ///---
    keystate() {
        std::fill(m_keydown.begin(), m_keydown.end(), false);
        std::fill(m_keytimes.begin(), m_keytimes.end(), 0);
    }

    // Change the specified keys state to pressed
    void key_down(const keycode key)
    {
        m_keydown[key] = true;
    }

    // Change the specified keys state to up
    void key_up(const keycode key)
    {
        m_keydown[key] = false;
    }

    // Update all key timings
    void update_all(void)
    {
        for (int i = 0; i < keycode_length; ++i) {
            if (m_keydown[i])
                m_keytimes[i]++;
            else
                m_keytimes[i] = 0;
        }
    }

    // Return whether a key was pushed.
    // A key can only be pushed once on each key down event. If the key is
    // held down, this will return false on successive calls.
    bool is_pushed(const keycode key)
    {
        return m_keytimes[key] == 1;
    }

    // Return whether a key is pressed.
    // A key is pressed if it is in the down state.
    bool is_pressed(const keycode key)
    {
        return m_keytimes[key] >= 1;
    }

    // Return whether a key is pushed, or has been down longer than the
    // specified length of time. This is particularly useful for das
    // calculations.
    bool is_pressed_with_das(const keycode key, const int das)
    {
        return m_keytimes[key] == 1 ||
               m_keytimes[key] > das;
    }

    ///----------------
    // Member Variables
    ///---

    // Store the status of keys:
    //  - false => up
    //  - true  => down
    std::array<bool, keycode_length> m_keydown;

    // Store how long each key has been pressed for in terms of ticks
    std::array<int, keycode_length> m_keytimes;
};

} // namespace mpe
