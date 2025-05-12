#include <SFML/Graphics.hpp>
#include <iostream>

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Sanity Test - WSL SFML");

    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "Font load FAILED!" << std::endl;
        return 1;
    }

    sf::Text text("HELLO WSL", font, 48);
    text.setFillColor(sf::Color::Red);
    text.setPosition(200, 100);

    sf::CircleShape circle(80.f);
    circle.setFillColor(sf::Color::Green);
    circle.setPosition(300, 300);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear(sf::Color::White);
        window.draw(text);
        window.draw(circle);
        window.display();
    }

    return 0;
}
