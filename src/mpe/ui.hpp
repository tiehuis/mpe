///
// ui.hpp
//
// Specifies an example ui implementation. This is generic and does not do any
// special drawing based on the mode. This design needs to be revised and
// thought about before any implementation.
//
// It is likely that rules themselves will specify what information to draw,
// but this information should be left to the ui to display. If this could be
// down in a rule-agnostic way that would be ideal.

#pragma once

#include <array>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include "mpe/engine.hpp"
#include "mpe/keystate.hpp"
#include "mpe/utility.hpp"

namespace mpe {

struct key {
    sf::Keyboard::Key code;
    mpe::keycode index;
};

// Specifies a mapping between sfml keyboard event types and internal keycodes
// which are used as indices.
static constexpr std::array<key, mpe::keycode_length> keymap = {{
    { sf::Keyboard::Left, mpe::keycode::left },
    { sf::Keyboard::Right, mpe::keycode::right },
    { sf::Keyboard::Down, mpe::keycode::down },
    { sf::Keyboard::Up, mpe::keycode::up },
    { sf::Keyboard::Space, mpe::keycode::space },
    { sf::Keyboard::Z, mpe::keycode::z },
    { sf::Keyboard::X, mpe::keycode::x },
    { sf::Keyboard::Q, mpe::keycode::q },
    { sf::Keyboard::C, mpe::keycode::c }
}};

class ui
{
  public:
    // RenderWindow has no copy constructor
    ui(int framerate) : window(sf::VideoMode(800, 600), "mptet")
    {
        window.setFramerateLimit(framerate);
    }

    ~ui()
    {
        window.close();
    }

    // Update the currently set keys
    void update(mpe::engine &engine)
    {
        for (auto key : keymap) {
            if (sf::Keyboard::isKeyPressed(key.code))
                engine.keystate.key_down(key.index);
            else
                engine.keystate.key_up(key.index);
        }
    }

    void render(const mpe::engine &engine)
    {
        window.clear(sf::Color::Black);
        render_field(engine);
        render_preview(engine);
        render_hold(engine);
        window.display();
    }

    void render_field(const mpe::engine &engine)
    {
        // Filled block
        sf::RectangleShape rfill(sf::Vector2f(15, 15));

        // Ghost outline
        sf::RectangleShape rline(sf::Vector2f(13, 13));
        rline.setFillColor(sf::Color::Transparent);
        rline.setOutlineThickness(1);
        rline.setOutlineColor(sf::Color::Blue);

        // Border
        sf::RectangleShape border(sf::Vector2f(15 * 10, 26 * 15));
        border.setFillColor(sf::Color::Transparent);
        border.setPosition(sf::Vector2f(250, 125));
        border.setOutlineThickness(1);
        window.draw(border);

        for (int y = 0; y < engine.field.height; ++y) {
            for (int x = 0; x < engine.field.width; ++x) {
                rfill.setPosition(sf::Vector2f(250 + 15 *x , 500 -15 * y));
                rline.setPosition(sf::Vector2f(250 + 15 * x + 1, 500 - 15 * y - 1));

                if (engine.field.at(x, y) || engine.block.at(x, y))
                    window.draw(rfill);
                else if (engine.ghost.at(x, y))
                    window.draw(rline);
            }
        }
    }

    void render_preview(const mpe::engine &engine) {}

    void render_hold(const mpe::engine &engine) {}

    ///----------------
    // Member Variables
    ///---

    sf::RenderWindow window;
};

} // namespace mpe
