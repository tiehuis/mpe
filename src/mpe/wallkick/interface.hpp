#pragma once

namespace mpe::wallkick {

struct Result {
    int x, y;
};

class Wallkick
{
  public:
    virtual ~Wallkick() = default;

    virtual int count(const int) const = 0;

    virtual Result right(const int, const int, const int) const = 0;

    virtual Result left(const int, const int, const int) const = 0;
};

} /* namespace mpe::wallkick */
