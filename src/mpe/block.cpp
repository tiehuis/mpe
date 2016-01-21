#include <vector>

#include "mpe/field.hpp"
#include "mpe/block.hpp"
#include "mpe/utility.hpp"

namespace mpe {

// The initial C position a block is spawned at
static const int c_initial_x = 3;

// The initial Y position a block is spawned at
static const int c_initial_y = 21;

// Blocks are not necessarily composed of 4-pieces, so a dynarray is
// a suitable container for storage, and is better than a vector.
static const std::vector<point> c_block_data[7][4] = {
    /* I Block */
    {
        {{0, 1}, {1, 1}, {2, 1}, {3, 1}},
        {{2, 0}, {2, 1}, {2, 2}, {2, 3}},
        {{0, 1}, {1, 1}, {2, 1}, {3, 1}},
        {{1, 0}, {1, 1}, {1, 2}, {1, 3}}
    },
    /* T Block */
    {
        {{0, 1}, {1, 0}, {1, 1}, {2, 1}},
        {{1, 0}, {1, 1}, {1, 2}, {2, 1}},
        {{0, 1}, {1, 1}, {1, 2}, {2, 1}},
        {{0, 1}, {1, 0}, {1, 1}, {1, 2}}
    },
    /* L Block */
    {
        {{0, 1}, {1, 1}, {2, 0}, {2, 1}},
        {{1, 0}, {1, 1}, {1, 2}, {2, 2}},
        {{0, 1}, {0, 2}, {1, 1}, {2, 1}},
        {{0, 0}, {1, 0}, {1, 1}, {1, 2}}
    },
    /* J Block */
    {
        {{0, 0}, {0, 1}, {1, 1}, {2, 1}},
        {{1, 0}, {1, 1}, {1, 2}, {2, 0}},
        {{0, 1}, {1, 1}, {2, 1}, {2, 2}},
        {{0, 2}, {1, 0}, {1, 1}, {1, 2}}
    },
    /* S Block */
    {
        {{0, 1}, {1, 0}, {1, 1}, {2, 0}},
        {{1, 0}, {1, 1}, {2, 1}, {2, 2}},
        {{0, 2}, {1, 1}, {1, 2}, {2, 1}},
        {{0, 0}, {0, 1}, {1, 1}, {1, 2}}
    },
    /* Z Block */
    {
        {{0, 0}, {1, 0}, {1, 1}, {2, 1}},
        {{1, 1}, {1, 2}, {2, 0}, {2, 1}},
        {{0, 1}, {1, 1}, {1, 2}, {2, 2}},
        {{0, 1}, {0, 2}, {1, 0}, {1, 1}}
    },
    /* O Block */
    {
        {{1, 0}, {1, 1}, {2, 0}, {2, 1}},
        {{1, 0}, {1, 1}, {2, 0}, {2, 1}},
        {{1, 0}, {1, 1}, {2, 0}, {2, 1}},
        {{1, 0}, {1, 1}, {2, 0}, {2, 1}}
    }
};

block::block(const block_type id, const rotation_type r)
    : x(c_initial_x), y(c_initial_y), id(id), r(r)
{
    can_be_held = true;
    data = c_block_data[id][r];
}

bool block::collision(const field &field)
{
    for (int i = 0; i < data.size(); ++i) {
        const int xoffset =  x + data[i].x;
        const int yoffset =  y + data[i].y;

        if (xoffset >= field.width || xoffset < 0)
            return true;

        if (yoffset < 0)
            return true;

        if (field.at(xoffset, yoffset))
            return true;
    }

    return false;
}

bool block::move_left(const field &field)
{
    x -= 1;

    if (!collision(field)) {
        return true;
    }
    else {
        x += 1;
        return false;
    }
}

bool block::move_right(const field &field)
{
    x += 1;

    if (!collision(field)) {
        return true;
    }
    else {
        x -= 1;
        return false;
    }
}

bool block::move_down(const field &field)
{
    y -= 1;

    if (!collision(field)) {
        return true;
    }
    else {
        y += 1;
        return false;
    }
}

bool block::move_n(const field& field, const int xl, const int yl)
{
    x += xl;
    y += yl;

    if (!collision(field)) {
        return true;
    }
    else {
        x -= xl;
        y -= yl;
        return false;
    }
}

bool block::rotate_right(const field &field, const int xl, const int yl)
{
    r = rotate_clockwise(r);
    data = c_block_data[id][r];
    x += xl;
    y += yl;

    if (!collision(field)) {
        return true;
    }
    else {
        r = rotate_anticlockwise(r);
        data = c_block_data[id][r];
        x -= xl;
        y -= yl;
        return false;
    }
}

bool block::rotate_right(const field &field, const wallkick::interface &wt)
{
    for (int test = 0; test < wt.count(id); ++test) {
        const wallkick::result wr = wt.right(id, r, test);
        if (rotate_right(field, wr.x, wr.y)) {
            return true;
        }
    }

    return false;
}

bool block::rotate_left(const field &field, const int xl, const int yl)
{
    r = rotate_anticlockwise(r);
    data = c_block_data[id][r];
    x += xl;
    y += yl;

    if (!collision(field)) {
        return true;
    }
    else {
        r = rotate_clockwise(r);
        data = c_block_data[id][r];
        x -= xl;
        y -= yl;
        return false;
    }
}

bool block::rotate_left(const field &field, const wallkick::interface &wt)
{
    for (int test = 0; test < wt.count(id); ++test) {
        const wallkick::result wr = wt.left(id, r, test);
        if (rotate_left(field, wr.x, wr.y)) {
            return true;
        }
    }

    return false;
}

void block::hard_drop(const field &field)
{
    while (move_down(field)) {}
}

bool block::at(const int xl, const int yl) const
{
    for (int i = 0; i < data.size(); ++i) {
        const int bx =  x + data[i].x;
        const int by =  y + data[i].y;

        if (bx == xl && by == yl)
            return true;
    }

    return false;
}

} // namespace mpe
