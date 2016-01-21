///
// randomizer.hpp
//
// Specifies an interface for all randomizer objects. These objects manage
// upcoming pieces and provide mechanisms for searching preview pieces.

#pragma once

#include <random>

#include "mpe/block.hpp"

namespace mpe::randomizer {

class interface
{
  public:
    // Use default destructor for all classes
    virtual ~interface() = default;

    // Return the next pending piece from the randomizer
    virtual block next() = 0;

    // Return the maximum number of preview pieces that can be shown
    virtual int preview_count() const = 0;

    // Return a vector of the next incoming pieces. The length of this vector
    // will be the size returned by previewCount
    virtual std::vector<int> preview_pieces() = 0;

  protected:
    // Implicitly called by each subclass. It is usually required for a
    // randomizer to have access to a 0-6 int distribution.
    interface() {
        std::random_device rd;
        generator = std::mt19937(rd());
        dist = std::uniform_int_distribution<int>(0, 6);
    }

    // Return a random block. This cannot be overriden by base classes.
    virtual block random_block() final {
        return ::mpe::block(static_cast<::mpe::block_type>(dist(generator)));
    }

    std::mt19937 generator;
    std::uniform_int_distribution<int> dist;
};

} /* mpe::namespace randomizer */
