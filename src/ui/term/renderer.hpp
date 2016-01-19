#pragma once

namespace tr {

#include <cstdio>

static const char *homePosition = "\e[H";

static const char *saveCursor = "\e7";

static const char *restoreCursor = "\e8";

static const char *hideCursor = "\e[?25l";

static const char *viewCursor = "\e[?25h";

static const char *saveScreenAndClear = "\e[?47h";

static const char *restoreScreen = "\e[?47l";

static const char *squareFill = "\u25a0 ";

static const char *squareLine = "\u25a1 ";

static const char *squareEmpty = "  ";

static const char *verticalBorder = "\u2502";

static const char *horizontalBorder = "\u2500\u2500";

static const char *upAndLeftBorder = "\u2514";

static const char *upAndRightBorder = "\u2518";

static const char *nextRow = "\e[nB";

static const char *beginningOfRow = "\eE";

static const char *resetColor = "\e[31m";

static const char *clearScreen = "\e[2J\e[H";

static const char *nextLine = "\eE";

const char* moveRelative(const int x, const int y)
{
    static char buffer[32];
    std::snprintf(buffer, sizeof(buffer), "\e[%d%c\e[%d%c",
        +y, y < 0 ? 'A' : 'B', +x, x < 0 ? 'D' : 'C');
    return buffer;
}

const char* setColor(const int id)
{
    static char buffer[32];
    std::snprintf(buffer, sizeof(buffer), "\e[%dm", 31 + id);
    return buffer;
}

} /* namespace tr */
