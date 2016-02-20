///
// x11_window.cpp
//
// Provide some useful functions for finding X11 windows based on their PID.
// This is especially useful for intercepting window events of existing
// windows.

#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <cstring>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>

#include <ui/terminal/x11_window.hpp>

///
// Perform a recursive descent across all currently active X11 windows
// searching for windows with the specified pid that belong to the specified
// display.
//
// TODO: Check that the resulting window id is indeed correct.
static Window _find_window_by_pid(Display *display, Window window,
        const pid_t pid)
{
    Atom atom;
    if ((atom = XInternAtom(display, "_NET_WM_PID", True)) == None)
        return 0;

    Atom actual_type;
    int actual_format;
    unsigned long nitems, bytes_after;
    unsigned char *prop;

    // Query window property for pid - Failure if no _NET_WM_PID.
    if (XGetWindowProperty(display, window, atom, 0, 1024, False,
                AnyPropertyType, &actual_type, &actual_format, &nitems,
                &bytes_after, &prop))
        return 0;

    // Only retrieve pid if _NET_WM_PID was defined, else set to a value that
    // fails and try next window.
    pid_t wpid = -1;
    if (prop) {
        wpid = prop[1] * 256 + prop[0];
        XFree(prop);
    }

    if (wpid == pid) {
        return window;
    }
    // Check child windows
    else {
        Window root, parent, *children;
        unsigned int nchildren;

        if (!XQueryTree(display, window, &root, &parent, &children, &nchildren))
            return 0;

        // Iterate over all children checking their results. If we succeed,
        // recurse back up to the root, freeing the structures as we go.
        for (int i = 0; i < nchildren; ++i) {
            Window result = _find_window_by_pid(display, children[i], pid);
            // Match has propagated up, Free children and propagate
            if (result != 0) {
                XFree(children);
                return result;
            }
        }
        XFree(children);
    }

    // Could not find window with specified pid
    return 0;
}

Window find_window_by_pid(Display *display, const pid_t pid)
{
    return _find_window_by_pid(display, XDefaultRootWindow(display), pid);
}

pid_t get_parent_process_id(const pid_t child)
{
    // Read /proc/$child/status to determine the parent process id
    char buf[128];
    std::snprintf(buf, sizeof(buf), "/proc/%ld/status", (long) child);

    FILE *fd = std::fopen(buf, "r");
    // Invalid child process
    if (fd == NULL)
        return -1;

    while (std::fgets(buf, sizeof(buf), fd)) {
        if (std::strncmp(buf, "PPid", 4) == 0) {
            char *ptr = std::strchr(buf, ':');
            // Invalid proc file
            if (ptr == NULL)
                break;

            ptr++; // Skip the : character
            while (std::isspace(*ptr)) ptr++;

            char *endptr;
            long ppid = std::strtol(ptr, &endptr, 10);

            // We should have a trailing '\n' proceeding the value
            if (*endptr != '\n')
                break;

            std::fclose(fd);
            return ppid;
        }
    }

    std::fclose(fd);
    return -1;
}

#ifdef MAINN
int main(void)
{
    // Find out the window id of the terminal which called this binary
    Display *display = XOpenDisplay(0);

    // Terminal is parent of bash script which started this process
    const pid_t terminal_pid = get_parent_process_id(getppid());

    std::printf("%lu\n", terminal_pid);

    Window window = find_window_by_pid(display, terminal_pid);

    std::printf("%lu\n", window);

    XCloseDisplay(display);
}
#endif
