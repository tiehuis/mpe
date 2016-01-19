#include <algorithm>
#include <experimental/optional>
#include <memory>
#include <thread>

#include "mpe/block.hpp"
#include "mpe/field.hpp"
#include "mpe/randomizer/bag.hpp"
#include "mpe/wallkick/srs.hpp"

using namespace std;

enum {
    KLeft, KRight, KDown, KZ, KX, KSpace, KQ, KC
};

static constexpr int DAS = 8;

class BaseEngine
{
  public:
    BaseEngine() : m_block(0), m_running(true)
    {
        m_field = mpe::Field();
        m_bag = make_unique<mpe::randomizer::Bag>();
        m_wk = make_unique<mpe::wallkick::SRS>();
        m_block = m_bag->next();
    }

    void update()
    {
        /* Horizontal movement */
        if (m_keystate[KLeft] == 1 || m_keystate[KLeft] > DAS)
            m_block.moveLeft(m_field);
        else if (m_keystate[KRight] == 1 || m_keystate[KRight] > DAS)
            m_block.moveRight(m_field);

        /* Vertical movement */
        if (m_keystate[KDown] == 1 || m_keystate[KDown] > DAS)
            m_block.moveDown(m_field);

        /* Rotation */
        if (m_keystate[KZ] == 1)
            m_block.rotateLeft(m_field, *m_wk);
        if (m_keystate[KX] == 1)
            m_block.rotateRight(m_field, *m_wk);

        /* Hold piece */
        if (m_keystate[KC] == 1 && m_block.m_can_be_held) {
            if (!m_hold) {
                m_hold = mpe::Block(m_block.m_id, 0);
                m_block = m_bag->next();
            }
            else {
                swap(m_hold.value(), m_block);
            }
            m_block.m_can_be_held = false;
        }

        /* Hard drop */
        if (m_keystate[KSpace] == 1) {
            m_block.hardDrop(m_field);
            m_field.placeBlock(m_block);
            m_field.lineClear();
            m_block = m_bag->next();
        }

        if (m_keystate[KQ] == 1) {
            m_running = false;
        }
    }

    bool isRunning()
    {
        return m_running;
    }

    virtual void render() const = 0;

  protected:
    array<int, 8> m_keystate;
    unique_ptr<mpe::randomizer::Randomizer> m_bag;
    unique_ptr<mpe::wallkick::Wallkick> m_wk;
    mpe::Block m_block;
    experimental::optional<mpe::Block> m_hold;
    mpe::Field m_field;
    bool m_running;
};
