///
// bag.hpp
//
// Specifies a bag randomizer. This randomizer has a bag of size 7, which is
// then shuffled periodically when a bag empties. This bag guarantees that the
// longest cycle of a piece not appearing is at most 13 blocks.
//
// In order to provide a 7-piece preview at all times, the array size is
// doubled and segments of the array is shuffled as needed.

#pragma once

#include <algorithm>
#include <numeric>

#include "mpe/randomizer/interface.hpp"
#include "mpe/block.hpp"

namespace mpe::randomizer {

// This is fixed currently, but theoretically we could extend this bag type to
// any multiple of 7 easily via templates.
constexpr int N = 7;

class bag : public interface
{
  public:
    bag() : index(0)
    {
        shuffle(0);
        shuffle(N);
    }

    void shuffle(const int start)
    {
        auto begin = start ? data.begin() : data.begin() + N;
        auto end   = start ? data.begin() + N : data.end();
        std::iota(begin, end, 0);
        std::shuffle(begin, end, generator);
    }

    block next()
    {
        block random_block = block(static_cast<block_type>(data[index]));
        index = (index + 1) % (2*N);

        if (index % N == 0) {
            shuffle(index);
        }

        return random_block;
    }

    int preview_count() const
    {
        return N;
    }

    std::vector<int> preview_pieces()
    {
        std::vector<int> previews;
        for (int i = 0; i < N; ++i)
            previews.push_back(data[(index + i) % (2*N)]);

        return previews;
    }

  private:
    int index;
    std::array<rotation_type, 2*N> data;
};

} /* mpe::namespace randomizer */
