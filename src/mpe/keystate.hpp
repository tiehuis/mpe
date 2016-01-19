///
// keystate.hpp
//
// Specifies the state of a set of restricted inputs.

#pragma once

#include <algorithm>
#include <array>

namespace mpe {

class enum keycode {
    left, right, up, down, z, x, c, space,
    length // Number of elements in enum
}

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
        for (auto &key : m_keytimes) {
            if (m_keydown[key])
                key++;
            else
                key = 0;
        }
    }

    // Return whether a key was pushed.
    // A key can only be pushed once on each key down event. If the key is
    // held down, this will return false on successive calls.
    void is_pushed(const keycode key)
    {
        return m_keytimes[key] == 1;
    }

    // Return whether a key is pressed.
    // A key is pressed if it is in the down state.
    void is_pressed(const keycode key)
    {
        return m_keytimes[key] >= 1;
    }

    // Return whether a key is pushed, or has been down longer than the
    // specified length of time. This is particularly useful for das
    // calculations.
    void is_pressed_with_das(const keycode key, const int das)
    {
        return m_keytimes[key] == 1 || m_keytimes > das;
    }

    ///----------------
    // Member Variables
    ///---

    // Store the status of keys:
    //  - false => up
    //  - true  => down
    std::array<bool, keycode::length> m_keydown;

    // Store how long each key has been pressed for in terms of ticks
    std::array<int, keycode::length> m_keytimes;
};

} // namespace mpe
