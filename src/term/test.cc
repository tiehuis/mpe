#include <cstdlib>
#include <cstdio>

#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>

///
// Helper method for recursing the window stack for a window with pid = `pid`.
static Window _findWindowByPid(Display *display, Window window, const pid_t pid)
{
    Atom type;
    int format;
    unsigned long nitems;
    unsigned long bytes_after;
    unsigned char *prop_pid = nullptr;

    // Check current window
    if (XGetWindowProperty(display, window, pid, 0, 1, False,
                XA_CARDINAL, &type, &format, &nitems, &bytes_after, &prop_pid) == Success) {
        if (prop_pid != nullptr) {
            XFree(prop_pid);
            return window;
        }
    }

    // Check child windows
    Window result, root, parent, *child;
    unsigned int nchild;

    // Recurse into child windows
    if (XQueryTree(display, window, &root, &parent, &child, &nchild) != 0) {
        for (unsigned int i = 0; i < nchild; ++i) {
            Window result = _findWindowByPid(display, child[i], pid);
            if (result != 0) {
                XFree(child);
                return result;
            }
        }
        XFree(child);
    }

    return 0;
}

///
// Return a window matching the given pid.
Window getWindowByPid(Display *display, Window window, const pid_t pid)
{
    Atom atom_pid = XInternAtom(display, "_NET_WM_PID", True);
    if (atom_pid == None)
        return 0;

    return _findWindowByPid(display, window, pid);
}

///
// Return the window of the terminal which spawned this process.
Window getTerminalWindow(Display *display)
{
    // This returns the shell executing in the terminal
    const pid_t pid = getppid();

    // Find the parent of this to find the terminal


    std::printf("%lu\n", pid);
    return getWindowByPid(display, XDefaultRootWindow(display), pid);
}

int main(void)
{
    // Find out the window id of the terminal which called this binary
    Display *display = XOpenDisplay(0);

    // Get the window of the terminal that spawned this
    Window window = getTerminalWindow(display);

    std::printf("%lu\n", window);
}
