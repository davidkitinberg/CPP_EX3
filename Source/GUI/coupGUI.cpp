// davidkitinberg@gmail.com

#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include "../../Headers/Game.hpp"
#include "../../Headers/Player.hpp"

using namespace coup;

// Global game objects
Game game;
Player* currentPlayer = nullptr;

// Enum for managing UI state
enum class GUIState { MainMenu, AddPlayer, Game, SelectTarget, WinnerScreen };

// Helper function that matches actions with roles that can block them
std::vector<std::string> getBlockingRoles(const std::string& action) {
    if (action == "Tax") return {"Governor"};
    if (action == "Bribe") return {"Judge"};
    if (action == "Coup") return {"General"};
    if (action == "Arrest") return {"Spy"};
    return {};
}

// Function for handling real-time action blocking for the matching roles
bool isActionBlocked(const std::string& action, Player* currentPlayer, Player* targetPlayer,
                     const sf::Font& font, std::vector<std::string>& log, Game& game) {
    std::vector<std::string> blockingRoles = getBlockingRoles(action);

    for (const auto& role : blockingRoles) 
    {
        for (Player* p : game.getPlayers()) 
        {
            if (p == currentPlayer || p->role() != role || !p->isActive()) continue;

            std::ostringstream title;
            title << "Player " << currentPlayer->getName() << " is trying to use " << action;
            if (targetPlayer != nullptr)
                title << " on " << targetPlayer->getName();

            sf::RenderWindow blockWindow(sf::VideoMode(400, 200), "Block Decision");

            sf::Text titleText(title.str(), font, 18);
            titleText.setFillColor(sf::Color::Black);
            titleText.setPosition(20, 30);

            sf::Text passBtn("Pass", font, 20);
            passBtn.setFillColor(sf::Color::Green);
            passBtn.setPosition(80, 120);

            sf::Text blockBtn("Block", font, 20);
            blockBtn.setFillColor(sf::Color::Red);
            blockBtn.setPosition(220, 120);

            bool madeChoice = false;
            while (blockWindow.isOpen()) {
                sf::Event e;
                while (blockWindow.pollEvent(e)) {
                    if (e.type == sf::Event::Closed) blockWindow.close();

                    if (e.type == sf::Event::MouseButtonPressed) // Handle mouse press event
                    {
                        sf::Vector2f mouse(sf::Mouse::getPosition(blockWindow));

                        if (passBtn.getGlobalBounds().contains(mouse)) // User pressed "pass"
                        {
                            blockWindow.close();
                            madeChoice = true;
                            break;
                        }

                        if (blockBtn.getGlobalBounds().contains(mouse)) // User pressed "block"
                        {
                            // Ask Game class to handle any block-related consequences
                            if (game.handleBlockConsequences(action, p, currentPlayer, log)) 
                            {
                                std::ostringstream logMsg;
                                logMsg << p->getName() << " blocked " << action << " by " << currentPlayer->getName();
                                if (targetPlayer != nullptr)
                                    logMsg << " targeting " << targetPlayer->getName();
                                log.push_back(logMsg.str());

                                blockWindow.close();
                                return true;  // Valid block
                            } 
                            else 
                            {
                                log.push_back(p->getName() + " tried to block " + action + " but could not afford it.");
                                blockWindow.close();
                                return false;  // Failed block — allow next blocker
                            }
                        }
                    }
                }
                

                // Pass button frame
                sf::RectangleShape passFrame(sf::Vector2f(passBtn.getGlobalBounds().width + 20, passBtn.getGlobalBounds().height + 20));
                passFrame.setPosition(passBtn.getPosition().x - 10, passBtn.getPosition().y - 5);
                passFrame.setOutlineColor(sf::Color::Green);
                passFrame.setOutlineThickness(2);
                passFrame.setFillColor(sf::Color::Transparent);

                // Block button frame
                sf::RectangleShape blockFrame(sf::Vector2f(blockBtn.getGlobalBounds().width + 20, blockBtn.getGlobalBounds().height + 20));
                blockFrame.setPosition(blockBtn.getPosition().x - 10, blockBtn.getPosition().y - 5);
                blockFrame.setOutlineColor(sf::Color::Red);
                blockFrame.setOutlineThickness(2);
                blockFrame.setFillColor(sf::Color::Transparent);

                blockWindow.clear(sf::Color(235, 242, 250));
                blockWindow.draw(titleText);
                blockWindow.draw(passBtn);
                blockWindow.draw(blockBtn);
                blockWindow.draw(blockFrame);
                blockWindow.draw(passFrame);

                blockWindow.display();

                if (madeChoice) break;
            }
        }
    }

    return false;  // Not blocked
}


int main() {
    // Create main window
    sf::RenderWindow window(sf::VideoMode(900, 700), "Coup Game Interface");

    // Load font
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "Font load FAILED!" << std::endl;
        return 1;
    }

    // GUI state management
    GUIState state = GUIState::MainMenu;
    std::string currentInput;
    std::string pendingAction; // For actions that require a target
    std::vector<std::string> playerNames;
    std::vector<std::string> log; // Game log
    float logScrollOffset = 0; // For scrollable log

    // Text UI Elements

    // Special spy coin bottom (only to see coins)
    sf::Text spyCoinBtn("Spy Coins", font, 20);
    spyCoinBtn.setFillColor(sf::Color::Red);

    // Special action button for role-specific abilities
    sf::Text specialActionBtn("Special", font, 20);
    specialActionBtn.setFillColor(sf::Color::Red);
    specialActionBtn.setPosition(650, 450); // Will be repositioned later per role

    // Coup game title
    sf::Text title("Coup Game", font, 40);
    title.setFillColor(sf::Color(59, 76, 202));
    title.setPosition(280, 50);

    // Add player bottom
    sf::Text addPlayerBtn("Add Player", font, 28);
    addPlayerBtn.setFillColor(sf::Color::Blue);
    addPlayerBtn.setPosition(320, 150);

    // Start game bottom
    sf::Text startGameBtn("Start Game", font, 28);
    startGameBtn.setFillColor(sf::Color::Magenta);
    startGameBtn.setPosition(320, 210);

    // Input UI
    sf::Text inputPrompt("Enter name:", font, 24);
    inputPrompt.setFillColor(sf::Color::Black);
    inputPrompt.setPosition(250, 220);

    // Input field for the input prompt
    sf::Text inputField("", font, 28);
    inputField.setFillColor(sf::Color::Black);
    inputField.setPosition(250, 260);

    // Enter bottom
    sf::Text enterBtn("Enter", font, 24);
    enterBtn.setFillColor(sf::Color::Green);
    enterBtn.setPosition(250, 320);

    // cancel bottom
    sf::Text cancelBtn("Cancel", font, 24);
    cancelBtn.setFillColor(sf::Color::Red);
    cancelBtn.setPosition(350, 320);

    // New Game bottom - winner screen
    sf::Text newGameBtn("New Game", font, 30);
    newGameBtn.setFillColor(sf::Color::Green);
    newGameBtn.setPosition(300, 300);

    // Exit Game bottom - winner screen
    sf::Text exitBtn("Exit Game", font, 30);
    exitBtn.setFillColor(sf::Color::Red);
    exitBtn.setPosition(500, 300);

    // Winner text declaration
    sf::Text winnerText("", font, 32);
    winnerText.setFillColor(sf::Color(245, 176, 65));
    winnerText.setPosition(200, 200);

    // Action buttons (shown in game state)
    std::vector<std::string> actionNames = {"Gather", "Tax", "Bribe", "Arrest", "Sanction", "Coup"};
    std::vector<sf::Text> actionButtons;
    for (std::size_t i = 0; i < actionNames.size(); ++i) {
        sf::Text btn(actionNames[i], font, 24);
        btn.setFillColor(sf::Color(44, 62, 145));
        btn.setPosition(650, 100 + i * 50);
        actionButtons.push_back(btn);
    }

    

//////////////////////////////////////////////////////////////////////

    
    while (window.isOpen()) // Main event loop
    {
        sf::Event event;
        while (window.pollEvent(event)) 
        {
            if (event.type == sf::Event::Closed) window.close();

            // Handle winner screen 
            if (state == GUIState::WinnerScreen && event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2f mouse(sf::Mouse::getPosition(window));
                if (newGameBtn.getGlobalBounds().contains(mouse)) {
                    game = Game();
                    playerNames.clear();
                    log.clear();
                    currentPlayer = nullptr;
                    state = GUIState::MainMenu;
                }
                if (exitBtn.getGlobalBounds().contains(mouse)) {
                    window.close();
                }
            }

            // Handle text input for adding player names
            if (state == GUIState::AddPlayer && event.type == sf::Event::TextEntered) 
            {
                if (event.text.unicode == '\b' && !currentInput.empty())
                    currentInput.pop_back();
                else if (event.text.unicode < 128 && event.text.unicode != '\b')
                    currentInput += static_cast<char>(event.text.unicode);

                inputField.setString(currentInput);
            }

            // Scroll log with mouse wheel
            if (event.type == sf::Event::MouseWheelScrolled && state == GUIState::Game) {
                logScrollOffset -= event.mouseWheelScroll.delta * 20;
                logScrollOffset = std::max(0.f, logScrollOffset);
            }

            // Handle mouse click events
            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2f mouse(sf::Mouse::getPosition(window));

                // Handle special action bottom
                if (state == GUIState::Game && specialActionBtn.getGlobalBounds().contains(mouse)) {
                    std::string role = currentPlayer->role();
                    if (role == "Governor") 
                    {
                        pendingAction = "BlockTax";
                        state = GUIState::SelectTarget;
                    } 
                    else if (role == "Spy") {
                        pendingAction = "BlockArrest";
                        state = GUIState::SelectTarget;
                    }
                    else if (role == "General") 
                    {
                        pendingAction = "BlockCoup";
                        state = GUIState::SelectTarget;
                    } 
                    else if (role == "Judge") 
                    {
                        pendingAction = "BlockBribe";
                        state = GUIState::SelectTarget;
                    } 
                    else if (role == "Baron") // Baron has the only special action that is not targetable
                    {
                        pendingAction = "Invest";
                        game.handleTurnWithNoTarget(currentPlayer, pendingAction, log);
                        game.checkElimination(log);
                        currentPlayer = game.turn();
                        state = GUIState::Game;
                    }
                }

                // Handle special spy coin report bottom
                if (state == GUIState::Game && currentPlayer->role() == "Spy" && spyCoinBtn.getGlobalBounds().contains(mouse)) {
                    std::ostringstream coinInfo;
                    for (auto* p : game.getPlayers()) {
                        coinInfo << p->getName() << ": " << p->coins() << " coins\n";
                    }

                    sf::RenderWindow reportWindow(sf::VideoMode(400, 300), "Spy Report"); // Render coin report
                    sf::Text reportText(coinInfo.str(), font, 20);
                    reportText.setFillColor(sf::Color::Black);
                    reportText.setPosition(20, 20);

                    while (reportWindow.isOpen()) {
                        sf::Event repEvent;
                        while (reportWindow.pollEvent(repEvent)) {
                            if (repEvent.type == sf::Event::Closed)
                                reportWindow.close();
                        }

                        reportWindow.clear(sf::Color(235, 242, 250));
                        reportWindow.draw(reportText);
                        reportWindow.display();
                    }
                    log.push_back("Spy saw the coin report");
                }

                
                if (state == GUIState::MainMenu) // Handle main menu bottoms (add player, start game)
                {
                    
                    if (addPlayerBtn.getGlobalBounds().contains(mouse) && game.getPlayers().size() < 6) // Add player button
                    {
                        state = GUIState::AddPlayer;
                        currentInput = "";
                        inputField.setString("");
                    }
                    // Start game button (only if at least 2 players)
                    if (game.getPlayers().size() >= 2 && startGameBtn.getGlobalBounds().contains(mouse)) 
                    {
                        for (const auto& name : playerNames)
                            log.push_back("Added player: " + name);
                        log.push_back("Game started.");
                        currentPlayer = game.turn();
                        state = GUIState::Game;
                    }
                }

                // Handle add player screen bottoms logic - adding the player to the game with a random role
                else if (state == GUIState::AddPlayer) {
                    // Confirm adding player
                    if (enterBtn.getGlobalBounds().contains(mouse) && !currentInput.empty()) {
                        game.addPlayerWithRandomRole(currentInput);
                        playerNames.push_back(currentInput);
                        currentInput.clear();
                        state = GUIState::MainMenu;
                    }
                    // Cancel adding player
                    if (cancelBtn.getGlobalBounds().contains(mouse)) {
                        state = GUIState::MainMenu;
                    }
                }

                // Handle the main game bottoms
                else if (state == GUIState::Game) {

                    bool cancelFurtherEventHandling = false; // Special flag for coup handling

                    // If a player has at least 10 coins we force him to coup
                    if (currentPlayer->coins() >= 10) {
                        // Force player to perform Coup
                        for (std::size_t i = 0; i < actionButtons.size(); ++i) {
                            if (actionButtons[i].getGlobalBounds().contains(mouse)) {
                                std::string action = actionNames[i];
                                if (action != "Coup") { // Ignore all non-Coup actions
                                    log.push_back(currentPlayer->getName() + " has 10 or more coins and must Coup.");
                                    cancelFurtherEventHandling = true;
                                    break;
                                }

                                // If it's Coup, continue as usual
                                pendingAction = "Coup";
                                state = GUIState::SelectTarget;
                                break;
                            }
                        }
                    }

                    if (cancelFurtherEventHandling) {
                        break; // exit the pollEvent loop early for this frame
                    }

                    // Check if an action button was clicked
                    for (std::size_t i = 0; i < actionButtons.size(); ++i) {
                        if (actionButtons[i].getGlobalBounds().contains(mouse)) {
                            std::string action = actionNames[i];
                            // If action requires target, change state
                            if (action == "Arrest" || action == "Sanction" || action == "Coup" || action == "BlockBribe" || action == "BlockCoup" || action == "BlockTax") {
                                pendingAction = action;
                                state = GUIState::SelectTarget; // Go to select target state
                            } 
                            else // If its a non targetable action
                            {
                                if (!isActionBlocked(action, currentPlayer, nullptr, font, log, game)) {
                                        game.handleTurnWithNoTarget(currentPlayer, action, log);
                                        game.checkElimination(log);
                                } 
                                else 
                                {
                                    log.push_back("Action was blocked.");
                                }
                                currentPlayer = game.turn();
                                game.handleMerchantPassive(currentPlayer, log);
                            }
                        }
                    }
                }

                // Handle modal target selection click events
                else if (state == GUIState::SelectTarget) {
                    // Handle X (close) button
                    sf::FloatRect closeRect(620, 160, 30, 30); // X at top-right of modal
                    if (closeRect.contains(mouse)) {
                        state = GUIState::Game;
                        continue;
                    }

                    // Handle target buttons
                    const auto& players = game.getPlayers();
                    int buttonIndex = 0;
                    for (size_t i = 0; i < players.size(); ++i) 
                    {
                        if (players[i] != currentPlayer) 
                        {
                            float bx = 270.f;
                            float by = 200.f + buttonIndex * 50.f;
                            sf::FloatRect btnRect(bx - 10, by - 5, 200, 30);
                            if (btnRect.contains(mouse)) 
                            {

                                if (!isActionBlocked(pendingAction, currentPlayer, players[i], font, log, game)) {
                                    game.handleTurnWithTarget(currentPlayer, pendingAction, players[i], log);
                                    game.checkElimination(log);
                                } else {
                                    log.push_back("Action was blocked.");
                                }
                                currentPlayer = game.turn();
                                game.handleMerchantPassive(currentPlayer, log); // Each turn, the game check for passive Merchant ability
                                state = GUIState::Game;
                            }
                            ++buttonIndex;
                        }
                    }
                } // end of target selection click events
            } // end of mouse click events block
        } // end of while main loop window.pollEvent(event)

        // --- Winner check ---
        if (state == GUIState::Game || state == GUIState::SelectTarget) {
            Player* potentialWinner = nullptr;
            potentialWinner = game.winner();

            if (potentialWinner != nullptr && state != GUIState::WinnerScreen) {
                std::ostringstream winMsg;
                winMsg << "The player \"" << potentialWinner->getName() << "\" is the winner!";
                winnerText.setString(winMsg.str());
                state = GUIState::WinnerScreen;
            }
        }


        
        // --- Rendering Section ---
        window.clear(sf::Color(235, 242, 250));

        if (state == GUIState::MainMenu) // Main manu rendering section
        {
            window.draw(title);

            // Draw Add Player Button only if we dont exceed the legal amount
            if(playerNames.size() < 6)
            {
                sf::RectangleShape btnFrame(sf::Vector2f(addPlayerBtn.getGlobalBounds().width + 20, addPlayerBtn.getGlobalBounds().height + 18));
                btnFrame.setPosition(addPlayerBtn.getPosition().x - 10, addPlayerBtn.getPosition().y - 5);
                btnFrame.setOutlineColor(sf::Color::Blue);
                btnFrame.setOutlineThickness(2);
                btnFrame.setFillColor(sf::Color::Transparent);
                window.draw(btnFrame);
                window.draw(addPlayerBtn);
            }
            

            // Draw Start Game Button only if enough players
            if (playerNames.size() >= 2) 
            {
                sf::RectangleShape startFrame(sf::Vector2f(startGameBtn.getGlobalBounds().width + 20, startGameBtn.getGlobalBounds().height + 18));
                startFrame.setPosition(startGameBtn.getPosition().x - 10, startGameBtn.getPosition().y - 5);
                startFrame.setOutlineColor(sf::Color::Magenta);
                startFrame.setOutlineThickness(2);
                startFrame.setFillColor(sf::Color::Transparent);
                window.draw(startFrame);
                window.draw(startGameBtn);
            }
        }

        else if (state == GUIState::WinnerScreen) // Winner screen rendering section
        {
            window.draw(winnerText);

            sf::RectangleShape newGameFrame(sf::Vector2f(newGameBtn.getGlobalBounds().width + 20, newGameBtn.getGlobalBounds().height + 20));
            newGameFrame.setPosition(newGameBtn.getPosition().x - 10, newGameBtn.getPosition().y - 10);
            newGameFrame.setOutlineColor(sf::Color::Green);
            newGameFrame.setOutlineThickness(2);
            newGameFrame.setFillColor(sf::Color::Transparent);
            window.draw(newGameFrame);
            window.draw(newGameBtn);

            sf::RectangleShape exitFrame(sf::Vector2f(exitBtn.getGlobalBounds().width + 20, exitBtn.getGlobalBounds().height + 20));
            exitFrame.setPosition(exitBtn.getPosition().x - 10, exitBtn.getPosition().y - 10);
            exitFrame.setOutlineColor(sf::Color::Red);
            exitFrame.setOutlineThickness(2);
            exitFrame.setFillColor(sf::Color::Transparent);
            window.draw(exitFrame);
            window.draw(exitBtn);
        }

        else if (state == GUIState::AddPlayer) // Adding player screen rendering section
        {
            // Draw name input box
            window.draw(inputPrompt);
            sf::RectangleShape inputBox(sf::Vector2f(300, 40));
            inputBox.setPosition(inputField.getPosition().x - 10, inputField.getPosition().y - 5);
            inputBox.setFillColor(sf::Color(245, 245, 245));
            inputBox.setOutlineColor(sf::Color::Black);
            inputBox.setOutlineThickness(2);
            window.draw(inputBox);
            window.draw(inputField);

            // Enter button frame
            sf::RectangleShape enterBox(sf::Vector2f(enterBtn.getGlobalBounds().width + 20, enterBtn.getGlobalBounds().height + 18));
            enterBox.setPosition(enterBtn.getPosition().x - 10, enterBtn.getPosition().y - 5);
            enterBox.setFillColor(sf::Color::Transparent);
            enterBox.setOutlineColor(sf::Color::Green);
            enterBox.setOutlineThickness(2);
            window.draw(enterBox);
            window.draw(enterBtn);

            // Cancel button frame
            sf::RectangleShape cancelBox(sf::Vector2f(cancelBtn.getGlobalBounds().width + 20, cancelBtn.getGlobalBounds().height + 18));
            cancelBox.setPosition(cancelBtn.getPosition().x - 10, cancelBtn.getPosition().y - 5);
            cancelBox.setFillColor(sf::Color::Transparent);
            cancelBox.setOutlineColor(sf::Color::Red);
            cancelBox.setOutlineThickness(2);
            window.draw(cancelBox);
            window.draw(cancelBtn);
        }

        else if (state == GUIState::Game || state == GUIState::SelectTarget) // Draw current player stats and list of others
        {
            if (currentPlayer) // Draw current's player stats
            {
                std::ostringstream stats;
                stats << "Current Turn:\n" << "Name: " << currentPlayer->getName()
                      << "\nRole: " << currentPlayer->role()
                      << "\nCoins: " << currentPlayer->coins()
                      << "\n\nOther Players:";
                for (auto* p : game.getPlayers()) {
                    if (p != currentPlayer) stats << "\n Player name: " << p->getName() << "\n Player's role: " << p->role();
                }
                sf::Text statsText(stats.str(), font, 24);
                statsText.setFillColor(sf::Color::Black);
                statsText.setPosition(30, 20);
                window.draw(statsText);
            }

            
            if (state == GUIState::Game) // Draw action buttons if in Game state
            {
                for (auto& btn : actionButtons) {
                    sf::RectangleShape box(sf::Vector2f(btn.getGlobalBounds().width + 20, btn.getGlobalBounds().height + 18));
                    box.setPosition(btn.getPosition().x - 10, btn.getPosition().y - 5);
                    box.setOutlineColor(sf::Color::Black);
                    box.setOutlineThickness(1);
                    box.setFillColor(sf::Color::Transparent);
                    window.draw(box);
                    window.draw(btn);
                }
            }

            // Draw special action bottom for each role
            if (state == GUIState::Game && currentPlayer != nullptr) 
            {
                std::string role = currentPlayer->role();

                specialActionBtn.setString(""); // reset the action bottom each time for the sake of roles with no special actions

                if (role == "Governor") 
                {
                    specialActionBtn.setString("Block Tax");
                    specialActionBtn.setPosition(650, 100 + 6 * 50);
                } 
                else if (role == "Spy") 
                {
                    specialActionBtn.setString("Block Arrest");
                    specialActionBtn.setPosition(650, 100 + 6 * 50);
                    spyCoinBtn.setPosition(650, 100 + 7 * 50); // Set spy coin bottom under Block Arrest
                    sf::RectangleShape spyFrame(sf::Vector2f(spyCoinBtn.getGlobalBounds().width + 20, spyCoinBtn.getGlobalBounds().height + 18));
                    spyFrame.setPosition(spyCoinBtn.getPosition().x - 10, spyCoinBtn.getPosition().y - 5);
                    spyFrame.setOutlineColor(sf::Color::Red);
                    spyFrame.setOutlineThickness(1);
                    spyFrame.setFillColor(sf::Color::Transparent);
                    window.draw(spyFrame);
                    window.draw(spyCoinBtn);
                    
                } 
                else if (role == "General") 
                {
                    specialActionBtn.setString("Block Coup");
                    specialActionBtn.setPosition(650, 100 + 6 * 50);
                    
                } 
                else if (role == "Judge") 
                {
                    specialActionBtn.setString("Block Bribe");
                    specialActionBtn.setPosition(650, 100 + 6 * 50);
                    
                } 
                else if (role == "Baron") 
                {
                    specialActionBtn.setString("Invest");
                    specialActionBtn.setPosition(650, 100 + 6 * 50);  
                }
                if (!specialActionBtn.getString().isEmpty()) 
                {
                    sf::RectangleShape specialBox(sf::Vector2f(specialActionBtn.getGlobalBounds().width + 20, specialActionBtn.getGlobalBounds().height + 18));
                    specialBox.setPosition(specialActionBtn.getPosition().x - 10, specialActionBtn.getPosition().y - 5);
                    specialBox.setOutlineColor(sf::Color::Red);
                    specialBox.setOutlineThickness(1);
                    specialBox.setFillColor(sf::Color::Transparent);
                    window.draw(specialBox);
                    window.draw(specialActionBtn);
                }
            }

            // Draw target selection modal --> Players selection to target
            else if (state == GUIState::SelectTarget) 
            {
                // Modal background
                sf::RectangleShape modal(sf::Vector2f(400, 300));
                modal.setPosition(250, 150);
                modal.setFillColor(sf::Color(230, 230, 230));
                modal.setOutlineColor(sf::Color::Black);
                modal.setOutlineThickness(3);
                window.draw(modal);

                // Modal title
                sf::Text modalTitle("Choose a target:", font, 22);
                modalTitle.setFillColor(sf::Color(59, 76, 202));
                modalTitle.setPosition(270, 160); // Adjust position as needed
                window.draw(modalTitle);

                // X button
                sf::Text closeX("X", font, 24);
                closeX.setFillColor(sf::Color::Red);
                modal.setOutlineColor(sf::Color::Red);
                closeX.setOutlineThickness(1);
                closeX.setPosition(620, 160);
                window.draw(closeX);

                // Player buttons inside modal
                const auto& players = game.getPlayers();
                int buttonIndex = 0;
                for (size_t i = 0; i < players.size(); ++i) 
                {
                
                    if (players[i] != currentPlayer) 
                    {
                        sf::Text btn(players[i]->getName(), font, 20);
                        float bx = 270.f;
                        float by = 200.f + buttonIndex * 50.f;
                        btn.setPosition(bx, by);
                        btn.setFillColor(sf::Color(44, 62, 145));
                        window.draw(btn);

                        // Frame
                        sf::RectangleShape frame(sf::Vector2f(btn.getGlobalBounds().width + 18, btn.getGlobalBounds().height + 18));
                        frame.setPosition(bx - 10, by + 5);
                        frame.setOutlineColor(sf::Color::Black);
                        frame.setOutlineThickness(1);
                        frame.setFillColor(sf::Color::Transparent);
                        window.draw(frame);

                        ++buttonIndex;
                    }
                }
            }

            // Draw Log box, title, and scroll bar:

            sf::Text logTitle("Log events:", font, 20); // Log events title
            logTitle.setFillColor(sf::Color(59, 76, 202));
            logTitle.setPosition(50, 470);
            window.draw(logTitle);

            sf::RectangleShape logBg(sf::Vector2f(800, 150)); // Log box
            logBg.setFillColor(sf::Color(245, 248, 255));
            logBg.setOutlineColor(sf::Color::Black);
            logBg.setOutlineThickness(1);
            logBg.setPosition(50, 500);
            window.draw(logBg);

            // Handle scrolling logic
            float yStart = 515 - logScrollOffset;
            float lineHeight = 20;
            float boxTop = 510;
            float boxBottom = 500 + 150;

            for (std::size_t i = 0; i < log.size(); ++i) 
            {
                float y = yStart + i * lineHeight;
                float lineBottom = y + lineHeight;

                if (lineBottom < boxTop) continue; // Skip lines above box
                if (y > boxBottom - 1) break; // Stop if line starts below box

                sf::Text line(log[i], font, 16);
                line.setFillColor(sf::Color::Black);
                line.setPosition(60, y);
                window.draw(line);
            }

            // Handle scroll bar logic
            float scrollBarX = 50 + 800 - 10;
            float scrollBarY = 500;
            float scrollBarHeight = 150;
            float scrollThumbHeight = std::max(20.f, scrollBarHeight * (scrollBarHeight / std::max(scrollBarHeight, log.size() * 20.0f)));
            float scrollThumbY = scrollBarY + (logScrollOffset / std::max(1.f, log.size() * 20.0f - scrollBarHeight)) * (scrollBarHeight - scrollThumbHeight);
            scrollThumbY = std::min(scrollThumbY, scrollBarY + scrollBarHeight - scrollThumbHeight);

            sf::RectangleShape scrollTrack(sf::Vector2f(10, scrollBarHeight));
            scrollTrack.setPosition(scrollBarX, scrollBarY);
            scrollTrack.setFillColor(sf::Color(200, 200, 200));
            window.draw(scrollTrack);

            sf::RectangleShape scrollThumb(sf::Vector2f(10, scrollThumbHeight));
            scrollThumb.setPosition(scrollBarX, scrollThumbY);
            scrollThumb.setFillColor(sf::Color::Black);
            window.draw(scrollThumb);
        }

        window.display(); // Show everything
    }

    return 0;
}
