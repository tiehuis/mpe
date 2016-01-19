#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <mpe/block.hpp>
#include <mpe/field.hpp>
#include <mpe/randomizer/bag.hpp>
#include <mpe/wallkick/srs.hpp>
#include <experimental/optional>
#include <memory>

enum {
    KLeft, KRight, KDown, KZ, KX, KSpace, KQ, KC
};

static constexpr int DAS = 8;

struct KeyMap {
    sf::Keyboard::Key key;
    int index;
};

static constexpr KeyMap keymaps[8] = {
    { sf::Keyboard::Left, KLeft }, { sf::Keyboard::Right, KRight },
    { sf::Keyboard::Down, KDown }, { sf::Keyboard::Z, KZ },
    { sf::Keyboard::X, KX }, { sf::Keyboard::Space, KSpace },
    { sf::Keyboard::Q, KQ }, { sf::Keyboard::C, KC }
};

class Engine
{
  public:
    Engine() : m_block(0)
    {
        m_field = mpe::Field();
        m_bag = std::make_unique<mpe::randomizer::Bag>();
        m_wk = std::make_unique<mpe::wallkick::SRS>();
        m_block = m_bag->next();
    }

    void updateKeys()
    {
        for (auto keymap : keymaps) {
            if (sf::Keyboard::isKeyPressed(keymap.key))
                m_keystate[keymap.index]++;
            else
                m_keystate[keymap.index] = 0;
        }
    }

    void update(sf::RenderWindow &window)
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
        if (m_keystate[KC] == 1) {
            if (!m_hold) {
                m_hold = m_block;
                m_block = m_bag->next();
            }
            else {
                std::swap(m_hold.value(), m_block);
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
            window.close();
        }
    }

    void render(sf::RenderWindow &window) const
    {
        window.clear(sf::Color::Black);
        renderField(window);
        renderPreview(window);
        renderHold(window);
        window.display();
    }

  private:
    void renderField(sf::RenderWindow &window) const
    {
        sf::RectangleShape rfill(sf::Vector2f(15, 15));

        sf::RectangleShape rline(sf::Vector2f(13, 13));
        rline.setFillColor(sf::Color::Transparent);
        rline.setOutlineThickness(1);
        rline.setOutlineColor(sf::Color::Blue);

        sf::RectangleShape border(sf::Vector2f(15 * 10, 26 * 15));
        border.setFillColor(sf::Color::Transparent);
        border.setPosition(sf::Vector2f(250, 125));
        border.setOutlineThickness(1);
        window.draw(border);

        mpe::Block ghost(m_block);
        ghost.hardDrop(m_field);

        for (int y = 0; y < m_field.getHeight(); ++y) {
            for (int x = 0; x < m_field.getWidth(); ++x) {
                rfill.setPosition(sf::Vector2f(250 + 15 * x, 500 - 15 * y));
                rline.setPosition(sf::Vector2f(250 + 15 * x + 1, 500 - 15 * y + 1));

                if (m_field.getCell(x, y) || m_block.isSet(x, y))
                    window.draw(rfill);
                else if (ghost.isSet(x, y))
                    window.draw(rline);
            }
        }
    }

    void renderPreview(sf::RenderWindow &window) const
    {
    }

    void renderHold(sf::RenderWindow &window) const
    {
    }

    std::array<int, 8> m_keystate;
    std::unique_ptr<mpe::randomizer::Randomizer> m_bag;
    std::unique_ptr<mpe::wallkick::Wallkick> m_wk;
    mpe::Block m_block;
    std::experimental::optional<mpe::Block> m_hold;
    mpe::Field m_field;
};

void render(mpe::Field &field, mpe::Block &block, sf::RenderWindow &window,
        mpe::randomizer::Randomizer &bag)
{
#if 0
    int xs = 450;
    int ys = 0;
    float scale_factor = 0.7;

    int z = 0;

    for (auto previewID : bag.previewPieces()) {
        auto block = mpe::Block(previewID);

        for (int i = 0; i < 4; ++i) {
            const int x = block.m_x + block.m_data[i].x;
            const int y = block.m_y + block.m_data[i].y;

            rfill.setPosition(sf::Vector2f(xs + scale_factor * 15 * x,
                        ys + z * 15 * 4 + scale_factor * 15 * y));
            window.draw(rfill);
        }

        ++z;
    }
#endif
}

int main(void)
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "mptet");
    window.setFramerateLimit(60);

    Engine engine;

    while (window.isOpen()) {
        engine.updateKeys();
        engine.update(window);
        engine.render(window);
        sf::sleep(sf::milliseconds(16));
    }

    return 0;
}
