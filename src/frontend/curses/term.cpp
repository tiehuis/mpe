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

#include "keyboard.hpp"

#include <ncurses.h>

static const char *squareFill = "\u25a0 ";

static const char *squareLine = "\u25a1 ";

static const char *squareEmpty = "  ";

static const char *verticalBorder = "\u2502";

static const char *horizontalBorder = "\u2500\u2500";

static const char *upAndLeftBorder = "\u2514";

static const char *upAndRightBorder = "\u2518";

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

    void render() const
    {
        renderField();
        refresh();
    }

    bool isRunning()
    {
        return m_running;
    }

  private:
    void renderField() const
    {
        /* Draw preview pieces and give a position to draw hold */
        mpe::Block ghost(m_block);
        ghost.hardDrop(m_field);

        for (int y = m_field.getHeight() - 1; y >= 0; --y) {
            mvaddstr(y, 0, verticalBorder);

            for (int x = 0; x < m_field.getWidth(); ++x) {
                if (m_field.getCell(x, y)) {
                    //nw.attron(COLOR_PAIR(m_field.getCell(x, y) - 1));
                    addstr(squareFill);
                    //nw.attroff(COLOR_PAIR(m_field.getCell(x, y) - 1));
                }
                else if (m_block.isSet(x, y)) {
                    //nw.attron(COLOR_PAIR(m_block.m_id));
                    addstr(squareFill);
                    //nw.attroff(COLOR_PAIR(m_block.m_id));
                }
                else if (ghost.isSet(x, y)) {
                    //nw.attron(COLOR_PAIR(m_block.m_id));
                    addstr(squareLine);
                    //nw.attroff(COLOR_PAIR(m_block.m_id));
                }
                else {
                    addstr(squareEmpty);
                }
            }

            addstr(verticalBorder);
        }

        mvaddstr(m_field.getHeight(), 0, upAndLeftBorder);
        for (int x = 0; x < m_field.getWidth(); ++x) {
            addstr(horizontalBorder);
        }
        addstr(upAndRightBorder);
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

void initCurses()
{
    setlocale(LC_ALL, "en_US.UTF-8");
    initscr();
    nodelay(stdscr, 1);
    keypad(stdscr, 1);
    noecho();
}

int main(void)
{
    Engine engine;
    initCurses();

    while (engine.isRunning()) {
        engine.updateKeys();
        engine.update();
        engine.render();

        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    endwin();
}
