#include <mpe/field.hpp>
#include <mpe/block.hpp>

namespace mpe {

static const int INITIAL_X = 3;

static const int INITIAL_Y = 21;

static const Point BLOCK_DATA[7][4][4] = {
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

Block::Block(const int id, const int r)
{
    m_id = id;
    m_r = r;
    m_x = INITIAL_X;
    m_y = INITIAL_Y;
    m_can_be_held = true;
    m_should_be_placed =false;
    m_data = (Point*) BLOCK_DATA[m_id][m_r];
}

bool Block::collision(const Field& field)
{
    for (int i = 0; i < 4; ++i) {
        const int x =  m_x + m_data[i].x;
        const int y =  m_y + m_data[i].y;

        if (x >= field.getWidth() || x < 0)
            return true;

        if (y < 0)
            return true;

        if (field.getCell(x, y))
            return true;
    }

    return false;
}

bool Block::moveLeft(const Field& field)
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

bool Block::moveRight(const Field& field)
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

bool Block::moveDown(const Field& field)
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

bool Block::moveN(const Field& field, const int x, const int y)
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

bool Block::rotateRight(const Field& field, const int x, const int y)
{
    m_r = (m_r + 3) % 4;
    m_data = (Point*) (BLOCK_DATA[m_id][m_r]);
    m_x += x;
    m_y += y;

    if (!collision(field)) {
        return true;
    }
    else {
        m_r = (m_r + 1) % 4;
        m_data = (Point*) (BLOCK_DATA[m_id][m_r]);
        m_x -= x;
        m_y -= y;
        return false;
    }
}

bool Block::rotateRight(const Field& field, const wallkick::Wallkick &wt)
{
    for (int test = 0; test < wt.count(m_id); ++test) {
        const wallkick::Result wr = wt.right(m_id, m_r, test);
        if (rotateRight(field, wr.x, wr.y)) {
            return true;
        }
    }

    return false;
}

bool Block::rotateLeft(const Field& field, const int x, const int y)
{
    m_r = (m_r + 1) % 4;
    m_data = (Point*) (BLOCK_DATA[m_id][m_r]);
    m_x += x;
    m_y += y;

    if (!collision(field)) {
        return true;
    }
    else {
        m_r = (m_r + 3) % 4;
        m_data = (Point*) (BLOCK_DATA[m_id][m_r]);
        m_x -= x;
        m_y -= y;
        return false;
    }
}

bool Block::rotateLeft(const Field& field, const wallkick::Wallkick &wt)
{
    for (int test = 0; test < wt.count(m_id); ++test) {
        const wallkick::Result wr = wt.left(m_id, m_r, test);
        if (rotateLeft(field, wr.x, wr.y)) {
            return true;
        }
    }

    return false;
}

void Block::hardDrop(const Field& field)
{
    while (moveDown(field)) {}
    m_should_be_placed = true;
}

bool Block::isSet(const int x, const int y) const
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
