#pragma once

#include <vector>

namespace mpe {

class Block;

const int DEFAULT_WIDTH = 10;

const int DEFAULT_HEIGHT = 25;

class Field
{
  public:
    Field(int = DEFAULT_WIDTH, int = DEFAULT_HEIGHT);

    int lineClear();

    void placeBlock(const Block &block);

    int getCell(const int, const int) const;

    int getHeight() const;

    int getWidth() const;

  private:
    std::vector<int> m_data;
    int m_w;
    int m_h;
};

} /* namespace mpe */
