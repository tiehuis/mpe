///
// rule/line_race.hpp
//
// Specifies the rule for a line race mode. In this mode, the player must
// reach the target goal as quickly as possible.

#pragma once

#include "mpe/rule/interface.hpp"
#include "mpe/statistics.hpp"

namespace mpe::rule {

class line_race
{
  public:
    ///----------------
    // Member Functions
    ///---

    line_race(int goal = 40) : m_goal(goal), m_cleared(0) {}

    bool end_condition() const
    {
        return m_cleared >= m_goal;
    }

    void update(const frame_statistics &fstat)
    {
        m_cleared += fstat.lines_cleared;
    }

    ///----------------
    // Member Variables
    ///---

    int m_goal;

    int m_cleared;
}

} // namespace rule
