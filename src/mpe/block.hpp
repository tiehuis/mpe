///
// block.hpp
//
// Specifies a block-type, i.e. a 'tetrimino' as is usually called in tetris.

#pragma once

#include <vector>
#include <memory>

#include "mpe/field.hpp"
#include "mpe/wallkick/interface.hpp"
#include "mpe/utility.hpp"

namespace mpe {

// This being a class enum complicates some things, but an explicit
// length field would ne nice. Consider changing.

typedef int block_type;

typedef int rotation_type;

//enum block_type {
//    I, T, L, J, S, Z, O,
//};

//enum rotation_type {
//    _0, _90, _180, _270
//};

class block
{
  public:
    ///----------------
    // Helper Functions
    ///---

    // Rotate clockwise by 90 degrees
    constexpr rotation_type rotate_clockwise(const rotation_type rt)
    {
        return (rt + 4 + 1) % 4;
    }

    // Rotate anti-clockwise by 90 degrees
    constexpr rotation_type rotate_anticlockwise(const rotation_type rt)
    {
        return (rt + 4 - 1) % 4;
    }

    ///----------------
    // Member Functions
    ///---

    // Create an instance with the given id and rotation. The default type of
    // I is arbitrary, and is to allow default initialization of blocks.
    block(const block_type id = 0, const rotation_type = 0);

    // Attempt to move the block left, returning true if success.
    bool move_left(const field &field);

    // Attempt to move the block right, returning true if success.
    bool move_right(const field &field);

    // Attempt to move the block down, returning true if success.
    bool move_down(const field &field);

    // Attempt to move the block as specified by the x, y coordinates.
    bool move_n(const field &field, const int x, const int y);

    // Attempt to rotate the block right, applying the given x, y offsets.
    bool rotate_right(const field &field, const int x = 0, const int y = 0);

    // Attempt to rotate the block right, applying a set of wallkick offsets as
    // defined by the given wallkick class.
    bool rotate_right(const field &field, const wallkick::interface &wt);

    // Attempt to rotate the block left, applying the given x, y offsets.
    bool rotate_left(const field &field, const int x = 0, const int y = 0);

    // Attempt to rotate the block left, applying a set of wallkick offsets as
    // defined by the given wallkick class.
    bool rotate_left(const field &field, const wallkick::interface &wt);

    // Attempt to drop the block all the way to the bottom of the given field.
    void hard_drop(const field &field);

    // Return whether the block occupies the given x, y coordinates.
    bool at(const int x, const int y) const;

    // Return whether the block collides with the field.
    bool collision(const field &field);

    ///----------------
    // Member Variables
    ///---

    // Current x-position of block
    int x;

    // Current y position of block
    int y;

    // Block id specifying which type this is
    block_type id;

    // Rotation state this block is in
    rotation_type r;

    // A non-reference makes implementation code cleaner but rotations require
    // an array copy. This is usually a small copy, however (max 4 points).
    std::vector<point> data;

    // Can the block be held?
    // This makes more sense in a higher-level API, such as engine
    bool can_be_held;
};

} // namespace mpe
