#include <algorithm>
#include <utility>
#include <mpe/block.hpp>
#include <mpe/field.hpp>

namespace mpe {

Field::Field(int w, int h)
{
    m_w = w;
    m_h = h;
    m_data.resize(m_w * m_h);
    std::fill(m_data.begin(), m_data.end(), 0);
}

int Field::lineClear()
{
    int cleared = 0;

    for (int y = 0; y < m_h; ++y) {
        auto row_begin = m_data.begin() + m_w * y;
        auto row_end = row_begin + m_w;

        if (std::count(row_begin, row_end, 0) == 0) {
            std::move(row_end, m_data.end(), row_begin);
            cleared += 1;
            y--;
        }
    }

    return cleared;
}

void Field::placeBlock(const Block &block)
{
    for (int i = 0; i < 4; ++i) {
        const int x = block.m_x + block.m_data[i].x;
        const int y = block.m_y + block.m_data[i].y;
        m_data[x + m_w * y] = block.m_id + 1;
    }
}

int Field::getCell(const int x, const int y) const
{
    return m_data[x + m_w * y];
}

int Field::getHeight() const
{
    return m_h;
}

int Field::getWidth() const
{
    return m_w;
}

} /* namespace mpe */
