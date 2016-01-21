///
// option.hpp
//
// Specifies a simple struct which stores all game-specific options. This
// is largely to avoid over-populating the main engine class.
//
// Convenience functions for storing data and parsing option files may be
// added at some stage here.

#pragma once

namespace mpe {

class option
{
  public:
    option() : are(0), das(8) {}

    ///----------------
    // Member Variables
    ///---

    int das;

    int are;
};

} // namespace mpe
