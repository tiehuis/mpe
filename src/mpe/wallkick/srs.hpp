///
// SRS.hpp
//
// Implements SRS rotation wallkicks.

#pragma once

#include <mpe/wallkick/interface.hpp>
#include <mpe/block.hpp>

namespace mpe::wallkick {

static const result d_right_JLSTZ[4][5] = {
    { {  0,  0 }, { -1,  0 }, { -1,  1 }, {  0, -2 }, { -1, -2 } }, // 0->R
    { {  0,  0 }, {  1,  0 }, {  1, -1 }, {  0,  2 }, {  1,  2 } }, // R->2
    { {  0,  0 }, {  1,  0 }, {  1,  1 }, {  0, -2 }, {  1, -2 } }, // 2->L
    { {  0,  0 }, { -1,  0 }, { -1, -1 }, {  0,  2 }, { -1,  2 } }  // L->0
};

static const result d_left_JLSTZ[4][5] = {
    { {  0,  0 }, {  1,  0 }, {  1,  1 }, {  0, -2 }, {  1, -2 } }, // 0->L
    { {  0,  0 }, { -1,  0 }, { -1, -1 }, {  0,  2 }, { -1,  2 } }, // L->2
    { {  0,  0 }, { -1,  0 }, { -1,  1 }, {  0, -2 }, { -1, -2 } }, // 2->R
    { {  0,  0 }, {  1,  0 }, {  1, -1 }, {  0,  2 }, {  1,  2 } }  // R->0
};

static const result d_right_I[4][5] = {
    { {  0,  0 }, { -2,  0 }, {  1,  0 }, { -2, -1 }, {  1,  2 } }, // 0->R
    { {  0,  0 }, { -1,  0 }, {  2,  0 }, { -1,  2 }, {  2, -1 } }, // R->2
    { {  0,  0 }, {  2,  0 }, { -1,  0 }, {  2,  1 }, { -1, -2 } }, // 2->L
    { {  0,  0 }, {  1,  0 }, { -2,  0 }, {  1, -2 }, {  2, -1 } }  // L->0
};

static const result d_left_I[4][5] = {
    { {  0,  0 }, { -1,  0 }, {  2,  0 }, { -1,  2 }, {  2, -1 } }, // 0->L
    { {  0,  0 }, { -2,  0 }, {  1,  0 }, { -2, -1 }, {  1,  2 } }, // L->2
    { {  0,  0 }, {  1,  0 }, { -2,  0 }, {  1, -2 }, { -2,  1 } }, // 2->R
    { {  0,  0 }, {  2,  0 }, { -1,  0 }, {  2,  1 }, { -1, -2 } }  // R->0
};

class SRS : public interface
{
  public:
    int count(const int id) const
    {
        static const int d_count[7] = { 5, 5, 5, 5, 5, 5, 1 };
        return d_count[id];
    }

    result left(const int id, const int br, const int test) const
    {
        if (id == mpe::block::I)
            return d_left_I[br][test];
        else
            return d_left_JLSTZ[br][test];
    }

    result right(const int id, const int br, const int test) const
    {
        if (id == mpe::block::I)
            return d_right_I[br][test];
        else
            return d_right_JLSTZ[br][test];
    }
};

} // namespace mpe::wallkick
