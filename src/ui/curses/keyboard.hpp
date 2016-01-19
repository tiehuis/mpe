#pragma once

/* We can hide c include files with an anonymous namespace to avoid potential
 * clashes in files which include this one. The same cannot be done for c++
 * includes due to possible linking errors. */
namespace {
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/input.h>
#include <termios.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
}

static const char *dev = "/dev/input/by-path/platform-i8042-serio-0-event-kbd";

/**
 * Implements a keyboard device which can be queried to get the status
 * of keys.
 */
class Keyboard
{
  public:
    /**
     * Attempt to open the keyboard device and the system. If this fails then
     * abort.
     */
    Keyboard()
    {
        m_fd = open(dev, O_RDONLY);
        if (m_fd == -1) {
            fprintf(stderr, "Failed to open: %s: %s\n",
                dev, strerror(errno));
            exit(1);
        }

        memset(m_keymap, 0, sizeof(m_keymap));

        /* Disable terminal input echo by default, since we will likely query
         * this class for determining key input. */
#if 0
        tcgetattr(STDIN_FILENO, &m_termset_old);
        m_termset_new = m_termset_old;
        m_termset_new.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &m_termset_new);
#endif
    }

    ~Keyboard()
    {
#if 0
        tcsetattr(STDIN_FILENO, TCSADRAIN, &m_termset_old);
#endif
    }

    /**
     * Return the number of new events read and update the keymap with newly
     * added events.
     */
    int readEvents() {
        int events_read = 0;
        int rd;
        struct input_event ev;

        /* Empty all current events then return */
        while (pendingEvent()) {
            if ((rd = read(m_fd, &ev, sizeof(ev))) < (int) sizeof(ev))
                exit(1);

            if (ev.type != EV_KEY || ev.value > 256)
                continue;

            switch (ev.value) {
            /* Key release */
            case 0:
                events_read++;
                m_keymap[ev.code] = 0;
                break;
            /* Key press */
            case 1:
                events_read++;
                m_keymap[ev.code]++;
                break;
            /* Auto-repeat */
            case 2:
                break;
            }
        }

        return events_read;
    }

    /**
     * Return if the given key is set. This should use the header values
     * located in linux/input.h.
     */
    int isSet(const int keycode)
    {
        return m_keymap[keycode];
    }

    /**
     * Return true if there is a pending request in the event queue.
     */
    bool pendingEvent()
    {
        struct timeval tv = {0, 0};
        fd_set fds;

        FD_ZERO(&fds);
        FD_SET(m_fd, &fds);
        select(FD_SETSIZE, &fds, NULL, NULL, &tv);
        return FD_ISSET(m_fd, &fds);
    }

  private:
    struct termios m_termset_old;
    struct termios m_termset_new;
    int m_fd;
    int m_keymap[256];
};
