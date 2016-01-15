#include <algorithm>
#include <chrono>
#include <thread>

#include "mpe/block.hpp"
#include "mpe/field.hpp"
#include "mpe/randomizer/bag.hpp"
#include "mpe/wallkick/srs.hpp"

#include "../engine.hpp"
#include "keyboard.hpp"
#include "renderer.hpp"

using namespace std;

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

class Engine : public BaseEngine
{
  public:
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

    void render() const override
    {
        std::cout << tr::clearScreen;
        std::cout << tr::resetColor;
        renderField();
        renderPreview();
        renderHold();
        std::cout.flush();
    }

  private:
    void renderBlock(mpe::Block &block) const
    {
        std::cout << tr::saveCursor;
        std::cout << tr::setColor(block.m_id);

        for (int i = 0; i < 4; ++i) {
            std::cout << tr::restoreCursor;

            const int x = 1 + 2 * block.m_data[i].x;
            const int y = 1 + block.m_data[i].y;

            std::cout << tr::moveRelative(x, y);
            std::cout << tr::squareFill;
        }

        std::cout << tr::resetColor;
        std::cout << tr::restoreCursor;
    }

    void renderHold() const
    {
        if (!m_hold)
            return;

        mpe::Block hold(m_hold.value().m_id);
        std::cout << tr::homePosition;
        renderBlock(hold);
    }

    void renderPreview() const
    {
        if (m_bag->previewCount() == 0)
            return;

        auto pieces = m_bag->previewPieces();

        for (int i = 0; i < std::min(m_bag->previewCount(), 4); ++i) {
            std::cout << tr::homePosition;
            std::cout << tr::moveRelative(32, i * 5);
            mpe::Block preview(pieces[i]);
            renderBlock(preview);
        }
    }

    void renderField() const
    {
        /* Draw preview pieces and give a position to draw hold */
        mpe::Block ghost(m_block);
        ghost.hardDrop(m_field);

        std::cout << tr::homePosition;

        for (int y = m_field.getHeight() - 1; y >= 0; --y) {
            std::cout << "          ";
            std::cout << tr::verticalBorder;
            for (int x = 0; x < m_field.getWidth(); ++x) {
                if (m_field.getCell(x, y)) {
                    std::cout << tr::setColor(m_field.getCell(x, y) - 1);
                    std::cout << tr::squareFill;
                    std::cout << tr::resetColor;
                }
                else if (m_block.isSet(x, y)) {
                    std::cout << tr::setColor(m_block.m_id);
                    std::cout << tr::squareFill;
                    std::cout << tr::resetColor;
                }
                else if (ghost.isSet(x, y)) {
                    std::cout << tr::setColor(m_block.m_id);
                    std::cout << tr::squareLine;
                    std::cout << tr::resetColor;
                }
                else {
                    std::cout << tr::squareEmpty;
                }
            }

            std::cout << tr::verticalBorder;
            std::cout << tr::nextLine;
        }

        std::cout << "          ";
        std::cout << tr::upAndLeftBorder;
        for (int x = 0; x < m_field.getWidth(); ++x) {
            std::cout << tr::horizontalBorder;
        }

        std::cout << tr::upAndRightBorder;
    }

    Keyboard m_keyboard;
};

using namespace std::literals::chrono_literals;

int main(void)
{
    Engine engine;

    std::cout << tr::hideCursor;
    std::cout << tr::saveScreenAndClear;

    while (engine.isRunning()) {
        auto next_time_point = std::chrono::steady_clock::now() + 16ms;

        engine.updateKeys();
        engine.update();
        engine.render();

        std::this_thread::sleep_until(next_time_point);
    }

    std::cout << tr::viewCursor;
    std::cout << tr::restoreScreen;

    return 0;
}
