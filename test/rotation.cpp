#include "mpe/field.hpp"
#include "mpe/block.hpp"

#include "test.hpp"

using namespace mpe;

void test1()
{

    initial_state = {{
        "#########"
    }};

    end_state = {{
        "         "
    }};

    test::set_block(field, block, initial_state);
    field.line_clear();
    test::assert_state(field, block, end_state);
}
