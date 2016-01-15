#pragma once

#include <array>
#include <mpe/field.hpp>
#include <mpe/wallkick/interface.hpp>

namespace mpe {

/* Make this into a class enum soon */
enum BlockType {
    I, T, L, J, S, Z, O
};

struct Point {
    int x;
    int y;
};

class Block
{
  public:
    Block(const int id, const int r = 0);

    bool moveLeft(const Field&);

    bool moveRight(const Field&);

    bool moveDown(const Field&);

    bool moveN(const Field&, const int, const int);

    bool rotateRight(const Field&, const int x = 0, const int y = 0);

    bool rotateRight(const Field&, const wallkick::Wallkick&);

    bool rotateLeft(const Field&, const int x = 0, const int y = 0);

    bool rotateLeft(const Field&, const wallkick::Wallkick&);

    void hardDrop(const Field&);

    bool isSet(const int, const int) const;

    bool collision(const Field& field);

//  private:
    Point* m_data;

    int m_id;

    int m_x;

    int m_y;

    int m_r;

    bool m_can_be_held;

    bool m_should_be_placed;
};

} /* namespace mpe */
