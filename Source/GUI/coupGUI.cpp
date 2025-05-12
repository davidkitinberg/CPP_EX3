#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include "../../Headers/Game.hpp"
#include "../../Headers/Player.hpp"

using namespace coup;

Game game;
Player* currentPlayer = nullptr;

enum class GUIState { MainMenu, AddPlayer, Game, SelectTarget };

int main() {
    sf::RenderWindow window(sf::VideoMode(900, 700), "Coup Game Interface");
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "Font load FAILED!" << std::endl;
        return 1;
    }

    GUIState state = GUIState::MainMenu;
    std::string currentInput;
    std::string pendingAction;
    std::vector<std::string> playerNames;
    std::vector<std::string> log;
    float logScrollOffset = 0;

    // UI Elements
    sf::Text title("Coup Game", font, 40);
    title.setFillColor(sf::Color::Black);
    title.setPosition(280, 50);

    sf::Text addPlayerBtn("Add Player", font, 28);
    addPlayerBtn.setFillColor(sf::Color::Blue);
    addPlayerBtn.setPosition(320, 150);

    sf::Text startGameBtn("Start Game", font, 28);
    startGameBtn.setFillColor(sf::Color::Magenta);
    startGameBtn.setPosition(320, 210);

    // Input
    sf::Text inputPrompt("Enter name:", font, 24);
    inputPrompt.setFillColor(sf::Color::Black);
    inputPrompt.setPosition(250, 220);

    sf::Text inputField("", font, 28);
    inputField.setFillColor(sf::Color::Black);
    inputField.setPosition(250, 260);

    sf::Text enterBtn("Enter", font, 24);
    enterBtn.setFillColor(sf::Color::Green);
    enterBtn.setPosition(250, 320);

    sf::Text cancelBtn("Cancel", font, 24);
    cancelBtn.setFillColor(sf::Color::Red);
    cancelBtn.setPosition(350, 320);

    std::vector<std::string> actionNames = {"Gather", "Tax", "Bribe", "Arrest", "Sanction", "Coup"};
    std::vector<sf::Text> actionButtons;
    for (int i = 0; i < actionNames.size(); ++i) {
        sf::Text btn(actionNames[i], font, 24);
        btn.setFillColor(sf::Color::Black);
        btn.setPosition(650, 100 + i * 50);
        actionButtons.push_back(btn);
    }

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();

            if (state == GUIState::AddPlayer && event.type == sf::Event::TextEntered) {
                if (event.text.unicode == '\b' && !currentInput.empty()) currentInput.pop_back();
                else if (event.text.unicode < 128 && event.text.unicode != '\b') currentInput += static_cast<char>(event.text.unicode);
                inputField.setString(currentInput);
            }

            if (event.type == sf::Event::MouseWheelScrolled && state == GUIState::Game) {
                logScrollOffset -= event.mouseWheelScroll.delta * 20;
                logScrollOffset = std::max(0.f, logScrollOffset);
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2f mouse(sf::Mouse::getPosition(window));

                if (state == GUIState::MainMenu) {
                    if (addPlayerBtn.getGlobalBounds().contains(mouse)) {
                        state = GUIState::AddPlayer;
                        currentInput = "";
                        inputField.setString("");
                    }
                    if (game.getPlayers().size() >= 2 && startGameBtn.getGlobalBounds().contains(mouse)) {
                        for (const auto& name : playerNames) {
                            log.push_back("Added player: " + name);
                        }
                        log.push_back("Game started.");
                        currentPlayer = game.turn();
                        state = GUIState::Game;
                    }
                }
                else if (state == GUIState::AddPlayer) {
                    if (enterBtn.getGlobalBounds().contains(mouse) && !currentInput.empty()) {
                        game.addPlayerWithRandomRole(currentInput);
                        playerNames.push_back(currentInput);
                        currentInput.clear();
                        state = GUIState::MainMenu;
                    }
                    if (cancelBtn.getGlobalBounds().contains(mouse)) {
                        state = GUIState::MainMenu;
                    }
                }
                else if (state == GUIState::Game) {
                    for (int i = 0; i < actionButtons.size(); ++i) {
                        if (actionButtons[i].getGlobalBounds().contains(mouse)) {
                            std::string action = actionNames[i];
                            if (action == "Arrest" || action == "Sanction" || action == "Coup") {
                                pendingAction = action;
                                state = GUIState::SelectTarget;
                            } else {
                                log.push_back("Action: " + action);
                            }
                        }
                    }
                }
                else if (state == GUIState::SelectTarget) {
                    const auto& players = game.getPlayers();
                    for (size_t i = 0; i < players.size(); ++i) {
                        if (players[i] != currentPlayer) {
                            sf::Text temp(players[i]->getName(), font, 20);
                            temp.setPosition(120, 180 + i * 30);
                            if (temp.getGlobalBounds().contains(mouse)) {
                                log.push_back("Action: " + pendingAction + " on " + players[i]->getName());
                                state = GUIState::Game;
                                break;
                            }
                        }
                    }
                }
            }
        }

        window.clear(sf::Color::White);

        if (state == GUIState::MainMenu) {
            window.draw(title);
            sf::RectangleShape btnFrame(sf::Vector2f(addPlayerBtn.getGlobalBounds().width + 20, addPlayerBtn.getGlobalBounds().height + 10));
            btnFrame.setPosition(addPlayerBtn.getPosition().x - 10, addPlayerBtn.getPosition().y - 5);
            btnFrame.setOutlineColor(sf::Color::Blue);
            btnFrame.setOutlineThickness(2);
            btnFrame.setFillColor(sf::Color::Transparent);
            window.draw(btnFrame);
            window.draw(addPlayerBtn);

            if (playerNames.size() >= 2) {
                sf::RectangleShape startFrame(sf::Vector2f(startGameBtn.getGlobalBounds().width + 20, startGameBtn.getGlobalBounds().height + 10));
                startFrame.setPosition(startGameBtn.getPosition().x - 10, startGameBtn.getPosition().y - 5);
                startFrame.setOutlineColor(sf::Color::Magenta);
                startFrame.setOutlineThickness(2);
                startFrame.setFillColor(sf::Color::Transparent);
                window.draw(startFrame);
                window.draw(startGameBtn);
            }
        }
        else if (state == GUIState::AddPlayer) {
            window.draw(inputPrompt);
            sf::RectangleShape inputBox(sf::Vector2f(300, 40));
            inputBox.setPosition(inputField.getPosition().x - 10, inputField.getPosition().y - 5);
            inputBox.setFillColor(sf::Color(245, 245, 245));
            inputBox.setOutlineColor(sf::Color::Black);
            inputBox.setOutlineThickness(2);
            window.draw(inputBox);
            window.draw(inputField);

            sf::RectangleShape enterBox(sf::Vector2f(enterBtn.getGlobalBounds().width + 20, enterBtn.getGlobalBounds().height + 10));
            enterBox.setPosition(enterBtn.getPosition().x - 10, enterBtn.getPosition().y - 5);
            enterBox.setFillColor(sf::Color::Transparent);
            enterBox.setOutlineColor(sf::Color::Green);
            enterBox.setOutlineThickness(2);
            window.draw(enterBox);
            window.draw(enterBtn);

            sf::RectangleShape cancelBox(sf::Vector2f(cancelBtn.getGlobalBounds().width + 20, cancelBtn.getGlobalBounds().height + 10));
            cancelBox.setPosition(cancelBtn.getPosition().x - 10, cancelBtn.getPosition().y - 5);
            cancelBox.setFillColor(sf::Color::Transparent);
            cancelBox.setOutlineColor(sf::Color::Red);
            cancelBox.setOutlineThickness(2);
            window.draw(cancelBox);
            window.draw(cancelBtn);
        }
        else if (state == GUIState::Game || state == GUIState::SelectTarget) {
            if (currentPlayer) {
                std::ostringstream stats;
                stats << "Current Turn:\n" << "Name: " << currentPlayer->getName() << "\nRole: " << currentPlayer->role() << "\nCoins: " << currentPlayer->coins() << "\n\nOther Players:";
                for (auto* p : game.getPlayers()) {
                    if (p != currentPlayer) stats << "\n" << p->getName();
                }
                sf::Text statsText(stats.str(), font, 24);
                statsText.setFillColor(sf::Color::Black);
                statsText.setPosition(100, 100);
                window.draw(statsText);
            }

            if (state == GUIState::Game) {
                for (auto& btn : actionButtons) {
                    sf::RectangleShape box(sf::Vector2f(btn.getGlobalBounds().width + 20, btn.getGlobalBounds().height + 10));
                    box.setPosition(btn.getPosition().x - 10, btn.getPosition().y - 5);
                    box.setOutlineColor(sf::Color::Black);
                    box.setOutlineThickness(1);
                    box.setFillColor(sf::Color::Transparent);
                    window.draw(box);
                    window.draw(btn);
                }
            }
            else if (state == GUIState::SelectTarget) {
                const auto& players = game.getPlayers();
                for (size_t i = 0; i < players.size(); ++i) {
                    if (players[i] != currentPlayer) {
                        sf::Text temp(players[i]->getName(), font, 20);
                        temp.setPosition(120, 180 + i * 30);
                        temp.setFillColor(sf::Color::Red);
                        window.draw(temp);
                    }
                }
            }

            sf::RectangleShape logBg(sf::Vector2f(800, 150));
            logBg.setFillColor(sf::Color(240, 240, 240));
            logBg.setOutlineColor(sf::Color::Black);
            logBg.setOutlineThickness(1);
            logBg.setPosition(50, 500);
            window.draw(logBg);

            float y = 510 - logScrollOffset;
            for (int i = 0; i < log.size(); ++i) {
                sf::Text line(log[i], font, 16);
                line.setFillColor(sf::Color::Black);
                line.setPosition(60, y);
                y += 20;
                if (y > 500 + 150) break;
                if (y > 500) window.draw(line);
            }
        }
        window.display();
    }
    return 0;
}