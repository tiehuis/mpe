#pragma once

#include <mpe/wallkick/interface.hpp>
#include <mpe/block.hpp>

namespace mpe::wallkick {

static const Result d_right_JLSTZ[4][5] = {
    { {  0,  0 }, { -1,  0 }, { -1,  1 }, {  0, -2 }, { -1, -2 } }, /* 0->R */
    { {  0,  0 }, {  1,  0 }, {  1, -1 }, {  0,  2 }, {  1,  2 } }, /* R->2 */
    { {  0,  0 }, {  1,  0 }, {  1,  1 }, {  0, -2 }, {  1, -2 } }, /* 2->L */
    { {  0,  0 }, { -1,  0 }, { -1, -1 }, {  0,  2 }, { -1,  2 } }  /* L->0 */
};

static const Result d_left_JLSTZ[4][5] = {
    { {  0,  0 }, {  1,  0 }, {  1,  1 }, {  0, -2 }, {  1, -2 } }, /* 0->L */
    { {  0,  0 }, { -1,  0 }, { -1, -1 }, {  0,  2 }, { -1,  2 } }, /* L->2 */
    { {  0,  0 }, { -1,  0 }, { -1,  1 }, {  0, -2 }, { -1, -2 } }, /* 2->R */
    { {  0,  0 }, {  1,  0 }, {  1, -1 }, {  0,  2 }, {  1,  2 } }  /* R->0 */
};

static const Result d_right_I[4][5] = {
    { {  0,  0 }, { -2,  0 }, {  1,  0 }, { -2, -1 }, {  1,  2 } }, /* 0->R */
    { {  0,  0 }, { -1,  0 }, {  2,  0 }, { -1,  2 }, {  2, -1 } }, /* R->2 */
    { {  0,  0 }, {  2,  0 }, { -1,  0 }, {  2,  1 }, { -1, -2 } }, /* 2->L */
    { {  0,  0 }, {  1,  0 }, { -2,  0 }, {  1, -2 }, {  2, -1 } }  /* L->0 */
};

static const Result d_left_I[4][5] = {
    { {  0,  0 }, { -1,  0 }, {  2,  0 }, { -1,  2 }, {  2, -1 } }, /* 0->L */
    { {  0,  0 }, { -2,  0 }, {  1,  0 }, { -2, -1 }, {  1,  2 } }, /* L->2 */
    { {  0,  0 }, {  1,  0 }, { -2,  0 }, {  1, -2 }, { -2,  1 } }, /* 2->R */
    { {  0,  0 }, {  2,  0 }, { -1,  0 }, {  2,  1 }, { -1, -2 } }  /* R->0 */
};

class SRS : public Wallkick
{
    int count(const int id) const
    {
        static const int d_count[7] = { 5, 5, 5, 5, 5, 5, 1 };
        return d_count[id];
    }

    Result left(const int id, const int br, const int test) const
    {
        return id == mpe::I ? d_right_I[br][test] : d_right_JLSTZ[br][test];
    }

    Result right(const int id, const int br, const int test) const
    {
        return id == mpe::I ? d_left_I[br][test] : d_left_JLSTZ[br][test];
    }
};

} /* namespace mpe::wallkick */
