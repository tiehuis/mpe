///
// linux_input.hpp
//
// A thing wrapper around the linux/input.h header. This allows us to have more
// fine-grained control over keyboard state than we could otherwise obtain with
// getc and friends, which do not tell us anything about the state of a key.

#pragma once

// File inclusions will usually require the enumeration definitions.
#include <linux/input.h>

class linux_input
{
  public:
    // Initialize the input state by reading the reqired device file.
    // If we could not open the device, throw an exception.
    linux_input();

    // Close any open file descriptors;
    ~linux_input();

    // Read events into the input buffer, returning the number of events
    // that were read. Return -1 on error.
    int read_events();

    // Return true if the given key is down.
    bool is_down(const int keycode) const;

    // Return true if an event is pending.
    bool is_event_pending() const;

  private:
    // The state of all keys
    bool keymap[256];

    // The file-descriptor of the given input device
    int input_device_fd;
};
