///
// test.hpp
//
// Defines helper methods for interacting with testing components.

namespace mpe::test {

// Consider using a template to specify non-standard widths
constexpr int W = 10;

// The field state uses this typedef to ensure that input rows match the
// required specification of a field.
typedef std::vector<std::array<char, W>> field_state;

// Check if the given coodinates lie exist ont the field and if so, that a
// block occupies the cell. If an explicit rotation is specified then set
// the input value to this.
static bool is_set(const field_state &state, int &value, const int x, const int y)
{
    if ((x >= 0 && x < W) && y >= 0) {
        if (state[y][x] == 'o') {
            return true;
        }
        else if (value != -1 && '0' <= state[y][x] && state[y][x] <= '3') {
            value = static_cast<int>(state[y][x] - '0');
            return true;
        }
    }

    return false;
}

// This just bruteforces through all possible block combinations on the field
// to determine the current block. It'd be interesting to alter this so that
// it uses block values but that isn't strictly necessary for the moment.
//
// We note that we always iterate from the bottom row, upwards from the left.
// This allows us to assume our start position block and try a simple algorithm
// to eliminate non-matches.
std::experimental::optional<mpe::block>
determine_block(const field_state &state, const int x, const int y)
{
    using std::experimental::optional;

    // An explicit rotation specifier. This is required to avoid ambiguity in
    // some specific cases where we cannot determine rotation.
    int er = -1;

#define IB(x, y) is_set(state, explicit_rotation, x, y)

    // I(0,2), T(2), L(2,3), J(1,2), S(0,2), Z(0,2), O(0,1,2,3,4)
    if (IB(x + 1, y)) {

        // I(0,2), T(2), L(2), J(2)
        if (IB(x + 2, y)) {

            // I(0,2)
            if (IB(x + 3, y)) {
                if (er == -1) {
                    return nullopt;
                }
                else {
                    return make_optional<mpe::block>(bI, er);
                }
            }

            // L(2)
            else if (IB(x + 2, y + 1)) {
                return make_optional<mpe::block>(bL, 2);
            }

            // J(2)
            else if (IB(x, y + 1)) {
                return make_optional<mpe::block>(bJ, 2);
            }

            // T(2)
            else if (IB(x + 1, y + 1)) {
                return make_optional<mpe::block>(bT, 2);
            }
        }

        // L(3), Z(0,2), O(0,1,2,3,4)
        else if (IB(x, y + 1)) {

            // L(3)
            if (IB(x, y + 2)) {
                return make_optional<mpe::block>(bL, 3);
            }

            // Z(0,2)
            else if (IB(x - 1, y + 1)) {
                if (er == -1) {
                    return nullopt;
                }
                else {
                    return make_optional<mpe::block>(bZ, er);
                }
            }

            // O(0,1,2,3,4)
            else if (IB(x + 1, y + 1)) {
                if (er == -1) {
                    return nullopt;
                }
                else {
                    return make_optional<mpe::block>(bO, er);
                }
            }
        }

        // J(1), S(0,2)
        else if (IB(x + 1, y + 1)) {

            // J(1)
            if (IB(x + 1, y + 2)) {
                return make_optional<mpe::block>(bJ, 1);
            }

            // S(0,2)
            else if (IB(x + 2, y + 1)) {
                if (er == -1) {
                    return nullpot;
                    abort();
                }
                else {
                    return make_optional<mpe::block>(bS, er);
                }
            }
        }
    }

    // I(1,3), T(1,3), L(0,1), J(0,3), S(1,3), Z(1,3)
    else if (IB(x, y + 1)) {

        // I(1,3), T(1,3), L(1,3), J(3)
        if (IB(x, y + 2)) {

            // I(1, 3)
            if (IB(x, y + 3)) {
                if (er == -1) {
                    return nullopt;
                }
                else {
                    return make_optional<mpe::block>(bI, er);
                }
            }

            // T(1)
            else if (IB(x, y + 1)) {
                return make_optional<mpe::block>(bT, 1);
            }

            // T(3)
            else if (IB(x + 2, y + 1)) {
                return make_optional<mpe::block>(bT, 3);
            }

            // L(1)
            else if (IB(x, y + 2)) {
                return make_optional<mpe::block>(bL, 1);
            }

            // L(3)
            else if (IB(x + 2, y)) {
                return make_optional<mpe::block>(bL, 3);
            }

            // J(3)
            else if (IB(x + 2, y + 2)) {
                return make_optional<mpe::block>(bJ, 3);
            }
        }

        // J(0), Z(1,3)
        else if (IB(x + 1, y + 1)) {

            // J(0)
            if (IB(x + 2, y + 1)) {
                return make_optional<mpe::block>(bJ, 0);
            }

            // Z(1,3)
            else if (IB(x + 1, y + 2)) {
                if (er == -1) {
                    return nullopt;
                }
                else {
                    return make_optional<mpe::block>(bJ, er);
                }
            }
        }

        // S(1,3)
        else if (IB(x - 1, y + 1)) {

            // S(1,3)
            if (IB(x - 1, y + 2)) {
                if (er == -1) {
                    return nullopt;
                }
                else {
                    return make_optional<mpe::block>(bS, er);
                }
            }
        }
    }

    return 0;

#undef IB
}

///
// Set the state of a game by an input field_state. The input field_state can
// be thought of simply as a string representing each cell on a field.
//
// In the case of different rotations appearing equivalent, we can specify
// explicitly the rotation type as a value in the grid and that will be used.
//
// ----------------------------------------
// Ambiguous, could be one of two rotations:
//
//  oooo
// ###
//
// ----------------------------------------
// Specifies this is rotation number 1 (0-indexed)
//
//  ooo1
// ###
///
void set_state(mpe::field &field, mpe::block &block, const field_state &state)
{
    bool calculated_block = false;
    std::fill(field.data.begin(), field.data.end(), 0);
    block.data.empty();

    // First entry is topmost row
    for (int y = state.size() - 1; y >= 0; --y) {
        for (int x = 0; x < W; ++x) {
            // Block and Field y values are reversed
            const int adj_y = state.size() - 1 - y;

            switch (state[y][x]) {
              case '#':
                field.data[x][y] = 1;
                break;
              case 'o':
                if (!calculated_block) {
                    auto block_wrapper = determine_block_id(state, x, y);
                    if (!block_wrapper) {
                        abort();
                    }
                    else {
                        block = block_wrapper.value();
                    }
                    calculated_block = true;
                }
                break;
              case ' ':
              default:
                break;
            }
        }
    }
}

// Print the current status of the field/block and some debug information.
void print_state(const mpe::field &field, const mpe::block &block)
{
}

// Assert that the given state is equal to the given string representation
bool assert_state(mpe::field &field, mpe::block &block, const field_state &state)
{
    for (int y = state.size() - 1; y >= 0; --y) {
        for (int x = 0; x < W; ++x) {
            const int adj_y = state.size() - 1 - y;

            switch (state[y][x]) {
              case '#':
                if (!(field.data[x][y] == 1 && block.data[x][y] == 0)) {
                    print_state(field, block);
                }
              case 'o':
              case 'O':
              case '0':
              case '1':
              case '2':
              case '3':
                assert(field.data[x][y] == 0 && block.data[x][y] == 1);
                break;
              case ' ':
              default:
                assert(field.data[x][y] == 0 && block.data[x][y] == 1);
            }
        }
    }
}

} // namespace mpe::test
