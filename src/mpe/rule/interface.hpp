///
// rule/interface.hpp
//
// Specifies functions that all rules must implement.

#pragma once

#include "mpe/statistics.hpp"

namespace mpe::rule {

class interface
{
  public:
    // Virtual destructor
    virtual ~interface() = 0;

    // Return true if the end condition was satisfied. This isn't the only
    // way to finish a game, but only rule-specific end conditions are
    // implemented here.
    virtual bool end_condition() const = 0;

    // Update the current rule with the given frame_statistics object.
    virtual void update(const frame_statistics &fstat) = 0;
}

} // namespace rule
