#include <algorithm>
#include <chrono>
#include <thread>

#include "mpe/block.hpp"
#include "mpe/field.hpp"
#include "mpe/randomizer/bag.hpp"
#include "mpe/wallkick/srs.hpp"

#include <SDL.h>
#include "../engine.hpp"

using namespace std;

struct KeyMap {
    int key;
    int index;
};

static constexpr KeyMap keymaps[8] = {
    { SDL_SCANCODE_LEFT, KLeft }, { SDL_SCANCODE_RIGHT, KRight },
    { SDL_SCANCODE_DOWN, KDown }, { SDL_SCANCODE_Z, KZ },
    { SDL_SCANCODE_X, KX }, { SDL_SCANCODE_SPACE, KSpace },
    { SDL_SCANCODE_Q, KQ }, { SDL_SCANCODE_C, KC }
};

class Engine : public BaseEngine
{
  public:
    Engine()
    {
        SDL_Init(SDL_INIT_VIDEO);
        m_window = SDL_CreateWindow("mptet", 0, 0, 0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP);
        m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    }

    ~Engine()
    {
        SDL_DestroyRenderer(m_renderer);
        SDL_DestroyWindow(m_window);
        SDL_Quit();
    }

    void updateKeys()
    {
        SDL_PumpEvents();
        const Uint8 *state = SDL_GetKeyboardState(NULL);

        for (auto keymap : keymaps) {
            if (state[keymap.key])
                m_keystate[keymap.index]++;
            else
                m_keystate[keymap.index] = 0;
        }
    }

    void render() const override
    {
        SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 0xff);
        SDL_RenderClear(m_renderer);

        renderField();
        renderPreview();
        renderHold();
        SDL_RenderPresent(m_renderer);
    }

  private:
    static constexpr int gridsize = 30;

    void drawRectangle(int x, int y) const
    {
        const SDL_Rect r = { x * gridsize, y * gridsize, gridsize, gridsize };
        SDL_RenderDrawRect(m_renderer, &r);
    }

    void fillRectangle(int x, int y) const
    {
        const SDL_Rect r = { x * gridsize, y * gridsize, gridsize, gridsize };
        SDL_RenderFillRect(m_renderer, &r);
    }

    void renderBlock(mpe::Block &block, int x, int y) const
    {
        for (int i = 0; i < 4; ++i) {
            const int xx = gridsize * (x + m_block.m_data[i].x);
            const int yy = gridsize * (y - block.m_data[i].y);

            SDL_SetRenderDrawColor(m_renderer, 0x80, 0x80, 0xff / 2, 0);
            fillRectangle(xx, yy);
        }
    }

    void renderHold() const
    {
        if (!m_hold)
            return;

        mpe::Block hold(m_hold.value().m_id);
        renderBlock(hold, 1, 1);
    }

    void renderPreview() const
    {
        if (m_bag->previewCount() == 0)
            return;

        auto pieces = m_bag->previewPieces();

        for (int i = 0; i < std::min(m_bag->previewCount(), 4); ++i) {
            mpe::Block preview(pieces[i]);
            renderBlock(preview, 15, i * 5);
        }
    }

    void renderField() const
    {
        /* Draw preview pieces and give a position to draw hold */
        mpe::Block ghost(m_block);
        ghost.hardDrop(m_field);

        /* 5 cell offset for start x position of field. */
        static const int offset = 5;

        for (int y = m_field.getHeight() - 1; y >= 0; --y) {
            for (int x = 0; x < m_field.getWidth(); ++x) {
                const int adjy = m_field.getHeight() - 1 - y;
                if (m_field.getCell(x, y)) {
                    SDL_SetRenderDrawColor(m_renderer, 0x80, 0x80, 0xff, 0xff);
                    fillRectangle(offset + x, adjy);
                }
                else if (m_block.isSet(x, y)) {
                    SDL_SetRenderDrawColor(m_renderer, 0x80, 0x80, 0xff / 2, 0);
                    fillRectangle(offset + x, adjy);
                }
                else if (ghost.isSet(x, y)) {
                    SDL_SetRenderDrawColor(m_renderer, 0x80, 0x80, 0xff, 0xff);
                    drawRectangle(offset + x, adjy);
                }
                else {
                    SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 0xff);
                    fillRectangle(offset + x, adjy);
                }
            }
        }
    }

    SDL_Renderer *m_renderer;
    SDL_Window *m_window;
};

using namespace std::literals::chrono_literals;

static Engine engine;

#ifdef __EMSCRIPTEN__
#include <emscripten.h>

void em_main_tick(void)
{
    engine.updateKeys();
    engine.update();
    engine.render();
}
#endif

int main(void)
{

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(em_main_tick, 60, 1);
#else
    while (engine.isRunning()) {
        auto next_time_point = std::chrono::steady_clock::now() + 16ms;

        engine.updateKeys();
        engine.update();
        engine.render();

        std::this_thread::sleep_until(next_time_point);
    }
#endif

    return 0;
}
