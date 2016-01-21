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
bool _is_set(const field_state &state, int &value, const int x, const int y)
{
    if ((x >= 0 && x < W) && y >= 0) {
        if (state[y][x] == 'o') {
            return true;
        }
        else if (value != -1 && '0' <= state[y][x] <= '3') {
            value = static_cast<int>(state[y][x] - '0');
            return true;
        }
    }

    return false;
}

// This just bruteforces through all possible block combinations on the field
// to determine the current block. It'd be interesting to optimize this, but it
// isn't a priority.
//
// We note that we always iterate from the bottom row, upwards from the left.
// This allows us to assume our start position block and try a simple algorithm
// to eliminate non-matches.
int determine_block_id(const field_state &state, const int x, const int y)
{
    int explicit_rotation = -1;

#define IB(x, y) _is_set(state, explicit_rotation, x, y)

    // I(0,2), T(2), L(2,3), J(1,2), S(0,2), Z(0,2), O(0,1,2,3,4)
    if (IB(x + 1, y))

        // I(0,2), T(2), L(2), J(2)
        if (IB(x + 2, y))

            // I(0,2)
            if (IB(x + 3, y))
                // Ambiguous rotation specified
                if (explicit_rotation == -1) {
                    abort();
                }
            }

            // L(2)
            else if (IB(x + 2, y + 1))

            // J(2)
            else if (IB(x, y + 1))
            }

            // T(2)
            else {
            }
        }

        // L(3), Z(0,2), O(0,1,2,3,4)
        else if (IB(x, y + 1)) {

            // L(3)
            if (IB(x, y + 2)) {
            }

            // Z(0,2)
            else if (IB(x - 1, y + 1)) {
            }

            // O(0,1,2,3,4)
            else if (IB(x + 1, y + 1)) {
            }
        }

        // J(1), S(0,2)
        else if (IB(x + 1, y + 1)) {

            // J(1)
            if (IB(x + 1, y + 2)) {
            }

            // S(0,2)
            else {
            }
        }
    }

    // I(1,3), T(0,1,3), L(0,1), J(0,3), S(1,3), Z(1,3)
    else if (IB(x, y + 1)) {
    }

#undef
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
                block = mpe::block(determine_block_type(state, x, y));
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

} // namespace mpe::test
