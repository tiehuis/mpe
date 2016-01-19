#include <cstdlib>
#include <cstdio>
#include <mpe/block.hpp>
#include <mpe/field.hpp>
#include <mpe/randomizer/bag.hpp>
#include <mpe/wallkick/srs.hpp>
#include <experimental/optional>
#include <memory>

#include <chrono>
#include <thread>


using namespace std;

enum {
    KLeft, KRight, KDown, KZ, KX, KSpace, KQ, KC
};

static constexpr int DAS = 8;

struct KeyMap {
    int key;
    int index;
};

static constexpr KeyMap keymaps[8] = {
    { KEY_LEFT, KLeft }, { KEY_RIGHT, KRight },
    { KEY_DOWN, KDown }, { KEY_Z, KZ },
    { KEY_X, KX }, { KEY_SPACE, KSpace },
    { KEY_Q, KQ }, { KEY_C, KC }
};

class Engine
{
  public:
    Engine() : m_block(0), m_running(true)
    {
        m_field = mpe::Field();
        m_bag = make_unique<mpe::randomizer::Bag>();
        m_wk = make_unique<mpe::wallkick::SRS>();
        m_block = m_bag->next();
    }

    void updateKeys()
    {
        m_keyboard.readEvents();
        for (auto keymap : keymaps) {
            if (m_keyboard.isSet(keymap.key))
                m_keystate[keymap.index]++;
            else
                m_keystate[keymap.index] = 0;
        }
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
                m_hold = mpe::Block(m_block.m_id);
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

    void render(TerminalRenderer &tr) const
    {
        renderField(tr);
    }

    bool isRunning()
    {
        return m_running;
    }

  private:
    void renderField(TerminalRenderer &tr) const
    {
        /* Draw preview pieces and give a position to draw hold */
        mpe::Block ghost(m_block);
        ghost.hardDrop(m_field);

        for (int y = m_field.getHeight() - 1; y >= 0; --y) {
            const int wy = m_field.getHeight() - 1 - y;

            tr.positionCursor(0, wy);
            tr.write(verticalBorder);

            for (int x = 0; x < m_field.getWidth(); ++x) {
                if (m_field.getCell(x, y)) {
                    tr.setColor(m_field.getCell(x, y) - 1);
                    tr.write(squareFill);
                    tr.resetColor();
                }
                else if (m_block.isSet(x, y)) {
                    tr.setColor(m_block.m_id);
                    tr.write(squareFill);
                    tr.resetColor();
                }
                else if (ghost.isSet(x, y)) {
                    tr.setColor(m_block.m_id);
                    tr.write(squareLine);
                    tr.resetColor();
                }
                else {
                    tr.write(squareEmpty);
                }
            }

            tr.write(verticalBorder);
        }

        tr.positionCursor(0, m_field.getHeight());
        tr.write(upAndLeftBorder);
        for (int x = 0; x < m_field.getWidth(); ++x) {
            tr.write(horizontalBorder);
        }
        tr.write(upAndRightBorder);
    }

    array<int, 8> m_keystate;
    unique_ptr<mpe::randomizer::Randomizer> m_bag;
    unique_ptr<mpe::wallkick::Wallkick> m_wk;
    mpe::Block m_block;
    experimental::optional<mpe::Block> m_hold;
    mpe::Field m_field;
    Keyboard m_keyboard;
    bool m_running;
};

int main(void)
{
    Engine engine;

    TerminalRenderer renderer;

    while (engine.isRunning()) {
        engine.updateKeys();
        engine.update();
        engine.render(renderer);

        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    return 0;
}
