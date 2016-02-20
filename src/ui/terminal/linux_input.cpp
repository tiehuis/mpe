///
// linux_input.cpp
//
// Implementation file for interfacing with the linux/input.h header. This is
// a thin abstraction over this device.
//
// A current problem is the inability to filter events based on which windows
// are in focus. This can be done using X11 (and likely will soon), but I'm not
// 100% on the plan for when not running an x-server, but using for example a
// screen multiplexer like tmux/screen.

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <ui/terminal/linux_input.hpp>

// TODO: Determine appropriate device procedurally
static constexpr char input_device_name[] =
    "/dev/input/by-path/platform-i8042-serio-0-event-kbd";

linux_input::linux_input()
{
    input_device_fd = open(input_device_name, O_RDONLY);

    // Handle the permission case seperately since this is quite common.
    if (input_device_fd == -1) {
        if (errno == EACCES) {
            std::fprintf(stderr,
                    "Insufficient permissions to open device file: %s\n"
                    "\tRoot permissions are required\n",
                    input_device_name);
        }
        else {
            std::perror("Error opening input device");
        }

        std::exit(1);
    }

    std::memset(keymap, 0, sizeof(keymap));
}

linux_input::~linux_input()
{
    const int status = close(input_device_fd);

    if (status == -1) {
        std::perror("Error closing input device");
        std::exit(1);
    }
}

int linux_input::read_events()
{
    struct input_event event;
    int bytes_read;
    int events_read = 0;

    /* Empty all current events then return */
    while (is_event_pending()) {
        bytes_read = read(input_device_fd, &event, sizeof(event));

        if (bytes_read == -1) {
            std::perror("Error reading from input device");
            std::exit(1);
        }
        else if (bytes_read < (int) sizeof(event)) {
            // This can occur if interrupted by a signal. For now, just stop
            // reading events this loop.
            // TODO: Determine best course of action to take.
            break;
        }

        // Discard non-input events or ones that are out of range
        if (event.type != EV_KEY || event.value > 256)
            continue;

        switch (event.value) {
          // Key release
          case 0:
            events_read += 1;
            keymap[event.code] = false;
            break;
          // Key press
          case 1:
            events_read += 1;
            keymap[event.code] = true;
            break;
          // Key autorepeat
          case 2:
          default:
            break;
        }
    }

    return events_read;
}

bool linux_input::is_down(const int keycode) const
{
    return keymap[keycode];
}

bool linux_input::is_event_pending() const
{
    struct timeval tv = {0, 0};
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(input_device_fd, &fds);
    select(FD_SETSIZE, &fds, nullptr, nullptr, &tv);
    return FD_ISSET(input_device_fd, &fds);
}
