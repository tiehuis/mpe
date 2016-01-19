#include <experimental/dynarray>

#include "mpe/field.hpp"
#include "mpe/block.hpp"
#include "mpe/utiliy.hpp"

namespace mpe {

// The initial C position a block is spawned at
static const int c_initial_x = 3;

// The initial Y position a block is spawned at
static const int c_initial_y = 21;

// Blocks are not necessarily composed of 4-pieces, so a dynarray is
// a suitable container for storage, and is better than a vector.
static const std::experimental::dynarray<point> c_block_data[7][4] = {
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
{
    m_id = id;
    m_r = r;
    m_x = c_initial_x;
    m_y = c_initial_y;
    m_can_be_held = true;
    m_should_be_placed =false;
    m_data = c_block_data[m_id][m_r];
}

bool Block::collision(const field& field)
{
    for (int i = 0; i < 4; ++i) {
        const int x =  m_x + m_data[i].x;
        const int y =  m_y + m_data[i].y;

        if (x >= field.width || x < 0)
            return true;

        if (y < 0)
            return true;

        if (field.at(x, y))
            return true;
    }

    return false;
}

bool block::move_left(const field& field)
{
    m_x -= 1;

    if (!collision(field)) {
        return true;
    }
    else {
        m_x += 1;
        return false;
    }
}

bool block::move_right(const field& field)
{
    m_x += 1;

    if (!collision(field)) {
        return true;
    }
    else {
        m_x -= 1;
        return false;
    }
}

bool block::move_down(const dield& field)
{
    m_y -= 1;

    if (!collision(field)) {
        return true;
    }
    else {
        m_y += 1;
        return false;
    }
}

bool block::move_n(const field& field, const int x, const int y)
{
    m_x += x;
    m_y += y;

    if (!collision(field)) {
        return true;
    }
    else {
        m_x -= x;
        m_y -= y;
        return false;
    }
}

bool block::rotate_right(const field& field, const int x, const int y)
{
    m_r = rotation_clockwise(m_r);
    m_data = c_block_data[m_id][m_r];
    m_x += x;
    m_y += y;

    if (!collision(field)) {
        return true;
    }
    else {
        m_r = rotation_anticlockwise(m_r);
        m_data = (Point*) (c_block_data[m_id][m_r]);
        m_x -= x;
        m_y -= y;
        return false;
    }
}

bool block::rotate_right(const field& field, const wallkick::wallkick &wt)
{
    for (int test = 0; test < wt.count(m_id); ++test) {
        const wallkick::result wr = wt.right(m_id, m_r, test);
        if (rotate_right(field, wr.x, wr.y)) {
            return true;
        }
    }

    return false;
}

bool block::rotate_left(const field& field, const int x, const int y)
{
    m_r = rotate_anticlockwise(m_r);
    m_data = c_block_data[m_id][m_r];
    m_x += x;
    m_y += y;

    if (!collision(field)) {
        return true;
    }
    else {
        m_r = rotate_clockwise(m_r);
        m_data = c_block_data[m_id][m_r];
        m_x -= x;
        m_y -= y;
        return false;
    }
}

bool block::rotate_left(const field& field, const wallkick::wallkick &wt)
{
    for (int test = 0; test < wt.count(m_id); ++test) {
        const wallkick::result wr = wt.left(m_id, m_r, test);
        if (rotate_left(field, wr.x, wr.y)) {
            return true;
        }
    }

    return false;
}

void block::hard_drop(const field& field)
{
    while (move_down(field)) {}
    m_should_be_placed = true;
}

bool block::at(const int x, const int y) const
{
    for (int i = 0; i < 4; ++i) {
        const int bx =  m_x + m_data[i].x;
        const int by =  m_y + m_data[i].y;

        if (bx == x && by == y)
            return true;
    }

    return false;
}

} /* namespace mpe */
