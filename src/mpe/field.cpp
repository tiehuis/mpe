#include <algorithm>
#include <utility>

#include "mpe/block.hpp"
#include "mpe/field.hpp"

namespace mpe {

field::field(const int w, const int h, const int hh) :
    width(w), height(h), hidden(hh)
{
    m_data.resize(width * (height + hidden));
    std::fill(m_data.begin(), m_data.end(), 0);
}

int field::line_clear()
{
    int cleared = 0;

    for (int y = 0; y < height + hidden; ++y) {
        auto row_begin = m_data.begin() + width * y;
        auto row_end = row_begin + width;

        if (std::count(row_begin, row_end, 0) == 0) {
            std::move(row_end, m_data.end(), row_begin);
            cleared += 1;
            y--;
        }
    }

    return cleared;
}

void field::place_block(const block &block)
{
    for (int i = 0; i < 4; ++i) {
        const int x = block.m_x + block.m_data[i].x;
        const int y = block.m_y + block.m_data[i].y;
        m_data[x + m_w * y] = block.m_id + 1;
    }
}

int field::at(const int x, const int y) const
{
    return m_data[x + width * y];
}

} /* namespace mpe */
