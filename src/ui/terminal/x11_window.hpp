///
// x11_window.hpp
//
// Find X11 windowid's based on a given PID.

#pragma once

#include <sys/types.h>
#include <X11/Xlib.h>

///
// Find and return the first window found which has the specified pid and is
// active on the specified display.
//
// Return 0 on failure.
Window find_window_by_pid(Display *display, const pid_t pid);

///
// Return the parent of the specified child process. If the process has no
// parent, then return -1.
pid_t get_parent_process_id(const pid_t child);
