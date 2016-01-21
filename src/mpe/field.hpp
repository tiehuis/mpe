///
// field.hpp
//
// Specifies a field which blocks can be played to.

#pragma once

#include <vector>

namespace mpe {

// Forward declare block
class block;

// Default width of field
static constexpr int c_default_width = 10;

// Default height of field
static constexpr int c_default_height = 25;

// Default hidden height of field
static constexpr int c_default_hidden = 3;

class field
{
  public:

    ///----------------
    // Member Functions
    ///---
    field(const int w = c_default_width, const int h = c_default_height,
          const int hh = c_default_hidden);

    // Clear all lines on the field, returning the number cleared.
    int line_clear();

    // Fix a block into the current field.
    void place_block(const block &block);

    // Return the status of the field at the specified co-ordinates
    int at(const int x, const int y) const;

    ///----------------
    // Member Variables
    ///---

    // Width of the field
    int width;

    // Height of the field (excludes hidden rows)
    int height;

    // Number of hidden rows in field
    int hidden;

    // Store the contents of the field. This is store in a single-dimension
    // array for allocator convenience. Field offsets are calculated manually.
    std::vector<int> data;
};

} // namespace mpe
