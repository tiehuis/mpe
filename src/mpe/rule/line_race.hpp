///
// rule/line_race.hpp
//
// Specifies the rule for a line race mode. In this mode, the player must
// reach the target goal as quickly as possible.

#pragma once

#include "mpe/rule/interface.hpp"
#include "mpe/statistics.hpp"

namespace mpe::rule {

class line_race : public interface
{
  public:
    ///----------------
    // Member Functions
    ///---

    line_race(const int goal = 40) : goal(goal), cleared(0) {}

    bool end_condition() const
    {
        return cleared >= goal;
    }

    void update(const frame_statistics &fstat)
    {
        cleared += fstat.lines_cleared;
    }

    ///----------------
    // Member Variables
    ///---

    int goal;

    int cleared;
};

} // namespace rule
