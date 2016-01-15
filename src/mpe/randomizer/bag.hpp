#pragma once

#include <mpe/randomizer/interface.hpp>
#include <mpe/block.hpp>
#include <algorithm>
#include <numeric>
#include <iostream>

namespace mpe::randomizer {

constexpr int N = 7;

class Bag : public Randomizer
{
  public:
    Bag() : m_index(0)
    {
        shuffle(0);
        shuffle(N);
    }

    void shuffle(const int start)
    {
        auto begin = start ? m_bag.begin() : m_bag.begin() + N;
        auto end   = start ? m_bag.begin() + N : m_bag.end();
        std::iota(begin, end, 0);
        std::shuffle(begin, end, m_generator);
    }

    Block next()
    {
        Block random_block = Block(m_bag[m_index]);
        m_index = (m_index + 1) % (2*N);

        if (m_index % N == 0) {
            std::cout << "Shuffling m_index = " << m_index << "\n";
            shuffle(m_index);
        }

        return random_block;
    }

    int previewCount() const
    {
        return N;
    }

    std::vector<int> previewPieces()
    {
        std::vector<int> previews;
        for (int i = 0; i < N; ++i)
            previews.push_back(m_bag[(m_index + i) % (2*N)]);

        return previews;
    }

  private:
    int m_index;
    std::array<int, 2*N> m_bag;
};

} /* mpe::namespace randomizer */
