#include <algorithm>
#include <utility>

#include "mpe/block.hpp"
#include "mpe/field.hpp"

namespace mpe {

field::field(const int w, const int h, const int hh) :
    width(w), height(h), hidden(hh)
{
    data.resize(width * (height + hidden));
    std::fill(data.begin(), data.end(), 0);
}

int field::line_clear()
{
    int cleared = 0;

    for (int y = 0; y < height + hidden; ++y) {
        auto row_begin = data.begin() + width * y;
        auto row_end = row_begin + width;

        if (std::count(row_begin, row_end, 0) == 0) {
            std::move(row_end, data.end(), row_begin);
            cleared += 1;
            y--;
        }
    }

    return cleared;
}

void field::place_block(const block &block)
{
    for (int i = 0; i < 4; ++i) {
        const int x = block.x + block.data[i].x;
        const int y = block.y + block.data[i].y;
        data[x + width * y] = block.id + 1;
    }
}

int field::at(const int x, const int y) const
{
    return data[x + width * y];
}

} /* namespace mpe */
