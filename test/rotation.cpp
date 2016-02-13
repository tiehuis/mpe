#include "mpe/field.hpp"
#include "mpe/block.hpp"

#include "test.hpp"

static mpe::block block;
static mpe::field field;

#define TEST_FUNCTION(x, y, fn)                     \
do {                                                \
    const mpe::test::field_state _istate = {{ x }}; \
    const mpe::test::field_state _fstate = {{ y }}; \
    mpe::test::set_state(field, block, _istate);    \
    fn;                                             \
    mpe::test::assert_state(field, block, _fstate); \
} while (0)

///
// Lineclear function tests
void t1()
{
    TEST_FUNCTION(
        "##########",

        "          ",

        field.line_clear()
    );
}

void t2()
{
    TEST_FUNCTION(
        "#########"
        "### #####"
        " ########"

        "#########",
        "         "
        "#########"
        "#########",

        field.line_clear()
    );
}

///
// Rotaton function tests

///
// T-spin rotation tests

int main(void)
{
    t1();
    t2();
}
