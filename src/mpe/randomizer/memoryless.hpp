///
// memoryless.hpp
//
// Implements a naive block generator, as in the original SNES tetris.

#pragma once

#include "mpe/randomizer/interface.hpp"
#include "mpe/block.hpp"

namespace mpe::randomizer {

class memoryless : public interface
{
  public:
    block next()
    {
        return random_block();
    }

    int preview_count() const
    {
        return 0;
    }

    std::vector<int> preview_pieces()
    {
        return std::vector<int>();
    }
};

} /* mpe::namespace randomizer */
