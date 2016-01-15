#pragma once

#include <mpe/randomizer/interface.hpp>
#include <mpe/block.hpp>

namespace mpe::randomizer {

class Memoryless : public Randomizer
{
  public:
    Block next()
    {
        return randomBlock();
    }

    int previewCount() const
    {
        return 0;
    }

    std::vector<int> previewPieces()
    {
        return std::vector<int>();
    }
};

} /* mpe::namespace randomizer */
