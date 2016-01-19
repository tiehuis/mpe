///
// wallkick.hpp
//
// Specifies a generic interface for returning a sequence of wallkick results.

#pragma once

#include "mpe/utility.hpp"

namespace mpe::wallkick {

// A wallkick result is simply a x, y coordinate pair.
typedef point result;

class interface
{
  public:
    // Use the default destructor
    virtual ~interface() = default;

    // Return the number of wallkick tests that exist for this piece
    virtual int count(const int) const = 0;

    // Return the right wallkick result for the given block, rotation and test
    virtual result right(const int, const int, const int) const = 0;

    // Return the left wallkick result for the given block, rotation and test
    virtual result left(const int, const int, const int) const = 0;
};

} /* namespace mpe::wallkick */
