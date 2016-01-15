#pragma once

#include <mpe/block.hpp>
#include <random>
#include <iostream>

namespace mpe::randomizer {

class Randomizer
{
  public:
    /**
     * Utilize the default constructor for all base classes.
     */
    virtual ~Randomizer() = default;

    /**
     * Return the next pending piece from the randomizer.
     */
    virtual Block next() = 0;

    /**
     * Return the maximum number of preview pieces that this randomizer
     * can show.
     */
    virtual int previewCount() const = 0;

    /**
     * Return a vector containing the pending pieces. This vector should
     * have a length equivalent to the value returned by previewCount.
     */
    virtual std::vector<int> previewPieces() = 0;

  protected:
    /**
     * This constructor is called implicitly by each subclass.
     */
    Randomizer() {
        std::random_device rd;
        m_generator = std::mt19937(rd());
        m_dist = std::uniform_int_distribution<int>(0, 6);
    }

    /**
     * Return a random block. Cannot be overridden by a base class.
     */
    virtual Block randomBlock() final {
        return Block(m_dist(m_generator));
    }

    std::mt19937 m_generator;
    std::uniform_int_distribution<int> m_dist;
};

} /* mpe::namespace randomizer */
